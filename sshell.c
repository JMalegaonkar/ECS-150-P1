#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "command.h"
#include "command_pipeline.h"
#include "command_stack.h"

#define CMDLINE_MAX 512

void execute_command(Command *command, int is_first_command)
{
        // Handle input redirection (only possible for first command in pipeline)
        if (is_first_command && command->expects_input_file)
        {
                if (command->input_file == NULL)
                {
                        fprintf(stderr, "Error: no input file\n");
                        return;
                }

                int fd = open(command->input_file, O_RDONLY, 0);
                if (fd < 0)
                {
                        fprintf(stderr, "Error: cannot open input file\n");
                        return;
                }

                dup2(fd, STDIN_FILENO);
        }

        // Populate argv to run execvp
        char *argv[command->args_len + 2];
        argv[0] = command->cmd;
        for (int i=0; i<command->args_len; i++)
        {
                argv[i+1] = command->args[i];
        }
        argv[command->args_len + 1] = NULL;

        // Run command using execvp
        execvp(command->cmd, argv);
        (errno == ENOENT)
                ? fprintf(stderr, "Error: command not found\n")
                : perror("execvp");
        exit(1);
}

void execute_pipeline_command(CommandPipeline* command_pipeline, const char* command_input)
{
        if (command_pipeline->commands_length == 1)
        {
                int pid = fork();
                if (pid == 0)
                {
                        execute_command(command_pipeline->commands[0], 1);
                        return;
                }
                int retval;
                waitpid(pid, &retval, 0);
                if (WIFEXITED(retval))
                {
                        fprintf(stderr, "+ completed '%s' [%d]\n", command_input, WEXITSTATUS(retval));
                }
                exit(0);
        }

        int pids[command_pipeline->commands_length];
        for (int i=1; i<command_pipeline->commands_length; i++)
        {
                int fd[2];
                pipe(fd);
                int pid = fork();
                if (pid == 0)
                {
                        // child
                        close(fd[0]);
                        dup2(fd[1], STDOUT_FILENO);
                        close(fd[1]);

                        int is_first_command = i-1 == 0;
                        execute_command(command_pipeline->commands[i-1], is_first_command);
                }
                else
                {
                        // parent
                        close(fd[1]);
                        dup2(fd[0], STDIN_FILENO);
                        close(fd[0]);

                        pids[i-1] = pid;

                        if (i == command_pipeline->commands_length-1)
                        {
                                pid = fork();
                                if (pid == 0)
                                {
                                        // child
                                        execute_command(command_pipeline->commands[i], 0);
                                }

                                // parent
                                pids[i] = pid;
                                char chained_status_codes[command_pipeline->commands_length * 3 + 1];
                                for (int i=0; i< command_pipeline->commands_length; i++)
                                {
                                        int retval;
                                        waitpid(pids[i], &retval, 0);

                                        chained_status_codes[3*i] = '[';
                                        chained_status_codes[3*i + 1] = '0' + WEXITSTATUS(retval);
                                        chained_status_codes[3*i + 2] = ']';
                                }
                                chained_status_codes[command_pipeline->commands_length * 3] = '\0';
                                fprintf(stderr, "+ completed '%s' %s\n", command_input, chained_status_codes);
                                exit(1);
                        }
                }
        }
}

int main(void)
{
        char command_input[CMDLINE_MAX];
        char cwd[PATH_MAX];

        // Creates command stack
        CommandStack* command_stack = create_stack(CMDLINE_MAX);

        while (1)
        {
                char *nl;
                int retval;

                // Print prompt
                fprintf(stdout, "sshell@ucd$ ");
                fflush(stdout);

                // Get command line
                fgets(command_input, CMDLINE_MAX, stdin);

                // Print command line if stdin is not provided by terminal
                if (!isatty(STDIN_FILENO))
                {
                        printf("%s", command_input);
                        fflush(stdout);
                }

                // Replace trailing newline from command line with null terminator
                nl = strchr(command_input, '\n');
                if (nl)
                {
                        *nl = '\0';
                }

                // Handle "exit" command
                if (!strcmp(command_input, "exit"))
                {

                        for (int i = 0; i < command_stack->top ; i ++)
                        {
                                free(command_stack->directory_stack[i]);
                        }

                        free(command_stack->directory_stack);
                        free(command_stack);

                        fprintf(stderr, "Bye...\n");
                        fprintf(stderr, "+ completed '%s' [0]\n", command_input);
                        exit(0);
                }

                // Handle "pwd" command
                if (!strcmp(command_input, "pwd"))
                {
                        getcwd(cwd, sizeof(cwd) * sizeof(char));
                        fprintf(stdout, "%s\n", cwd);
                        fprintf(stderr, "+ completed '%s' [0]\n", command_input);
                        continue;
                }

                // Parses command_input to create Command object
                CommandPipeline* command_pipeline = create_command_pipeline(command_input);
                if (command_pipeline == NULL)
                {
                        continue;
                }

                // Handles "cd" command
                int is_command_cd =
                        command_pipeline->commands_length == 1 &&
                        !strcmp(command_pipeline->commands[0]->cmd, "cd") &&
                        command_pipeline->commands[0]->args_len == 1;
                if (is_command_cd)
                {
                        int status_code = chdir(command_pipeline->commands[0]->args[0]);
                        if (status_code == -1)
                        {
                                fprintf(stderr, "Error: cannot cd into directory\n");
                        }
                        fprintf(stderr, "+ completed '%s' [%d]\n", command_input, status_code ? 1 : 0);
                        continue;
                }

                // Handle "dirs" command
                if (!strcmp(command_pipeline->commands[0]->cmd, "dirs"))
                {
                        get_commands(command_stack, getcwd(cwd, sizeof(cwd) * sizeof(char)));
                        fprintf(stderr, "+ completed '%s' [0]\n", command_input);

                        continue;
                }

                // Handle "pushd" command
                if (!strcmp(command_pipeline->commands[0]->cmd, "pushd"))
                {
                        getcwd(cwd, sizeof(cwd) * sizeof(char));
                        int status_code = chdir(command_pipeline->commands[0]->args[0]);
                        (status_code == -1)
                                ? fprintf(stderr, "Error: no such directory\n")
                                : push(command_stack, cwd);

                        fprintf(stderr, "+ completed '%s' [%d]\n", command_input, status_code ? 1 : 0);
                        continue;
                }

                // Handle "popd" command
                if (!strcmp(command_pipeline->commands[0]->cmd, "popd"))
                {
                        int status_code = (command_stack->top > -1) ? 0 : 1;
                        if (!status_code)
                        {
                                chdir(top(command_stack));
                        }
                        pop(command_stack);
                        fprintf(stderr, "+ completed '%s' [%d]\n", command_input, status_code);
                        continue;
                }


                // Executes fork + exec + wait loop to execute command
                if (fork() == 0) // child process
                {
                        // Handle output redirection
                        if (command_pipeline->output_file != NULL)
                        {
                                int fd = open(command_pipeline->output_file, O_TRUNC | O_WRONLY | O_CREAT, 0666);
                                dup2(fd, STDOUT_FILENO);
                        }

                        // Execute command
                        execute_pipeline_command(command_pipeline, command_input);
                        // if (command_pipeline->commands_length == 1)
                        // {
                        //         execute_command(command_pipeline->commands[0], 1);
                        // }
                        // else
                        // {
                        //         execute_pipeline_command(command_pipeline, command_input);
                        // }
                }
                else // parent process
                {
                        // Wait for child process to complete
                        wait(&retval);

                        if (!WIFEXITED(retval))
                        {
                                printf("exitted without code\n");
                                exit(0);
                        }

                        // Execute command
                        // WIFEXITED(retval)
                        //         ? fprintf(stderr, "+ completed '%s' [%d]\n", command_input, WEXITSTATUS(retval))
                        //         : fprintf(stderr, "Child did not terminate with exit\n");

                }

        }

        return EXIT_SUCCESS;
}