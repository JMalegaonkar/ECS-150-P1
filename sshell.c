#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "command.h"
#include "command_pipeline.h"
#include "command_stack.h"

#define CMDLINE_MAX 512
#define IS_FIRST_COMMAND 1
#define IS_NOT_FIRST_COMMAND 0
#define INPUT_REDIRECTION_ERROR_CODE 132

void execute_command(Command *command, int is_first_command)
{
        // Handle input redirection (only possible for first command in pipeline)
        if (is_first_command && command->expects_input_file)
        {
                if (command->input_file == NULL)
                {
                        fprintf(stderr, "Error: no input file\n");
                        exit(INPUT_REDIRECTION_ERROR_CODE);
                }

                int fd = open(command->input_file, O_RDONLY, 0);
                if (fd < 0)
                {
                        fprintf(stderr, "Error: cannot open input file\n");
                        exit(INPUT_REDIRECTION_ERROR_CODE);
                }

                dup2(fd, STDIN_FILENO);
        }

        // Populate argv to run execvp
        char *argv[command->args_len + 2];
        argv[0] = command->cmd;
        for (int i = 0; i < command->args_len; i++)
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

void execute_single_command(Command* command, const char* command_input)
{
        // Fork child to execute command
        if (fork() == 0)
        {
                execute_command(command, IS_FIRST_COMMAND);
        }

        // Grab return value and print completion message
        int retval;
        wait(&retval);
        if (WIFEXITED(retval) && WEXITSTATUS(retval) != INPUT_REDIRECTION_ERROR_CODE)
        {
                fprintf(stderr, "+ completed '%s' [%d]\n", command_input, WEXITSTATUS(retval));
        }
        exit(0);
}

void execute_final_pipelined_command(CommandPipeline* command_pipeline, int* pids, const char* command_input)
{
        int command_pipeline_length = command_pipeline->commands_length;

        // Fork child to execute final command
        pids[command_pipeline_length - 1] = fork();
        if (pids[command_pipeline_length - 1] == 0)
        {
                execute_command(command_pipeline->commands[command_pipeline_length - 1], IS_NOT_FIRST_COMMAND);
        }

        // Construct chained status code from all return values
        char chained_status_codes[command_pipeline_length * 5 + 1]; // 5 characters is largest status code (i.e. '[255]')
        int chained_status_codes_idx = 0;
        for (int i = 0; i < command_pipeline_length; i++)
        {
                // Grab status code from child process using pid
                int retval;
                waitpid(pids[i], &retval, 0);
                int status_code = WEXITSTATUS(retval);

                // Populate chain status code string with child process' status code
                chained_status_codes[chained_status_codes_idx++] = '[';

                sprintf(chained_status_codes + chained_status_codes_idx, "%d", status_code);
                int status_code_size = (status_code <= 1) ? 1 : (int)((ceil(log10(status_code)))*sizeof(char));
                chained_status_codes_idx += status_code_size;

                chained_status_codes[chained_status_codes_idx++] = ']';
        }
        chained_status_codes[chained_status_codes_idx] = '\0';

        // Print completion message with chained status code
        fprintf(stderr, "+ completed '%s' %s\n", command_input, chained_status_codes);
        exit(1);
}

void execute_pipelined_command(CommandPipeline* command_pipeline, const char* command_input)
{
        int command_pipeline_length = command_pipeline->commands_length;
        int pids[command_pipeline_length];

        // Execute all commands in different processes (using pipes to enable IPC)
        for (int command_idx = 1; command_idx < command_pipeline_length; command_idx++)
        {
                // Create pipe to enable IPC
                int fd[2];
                pipe(fd);

                // Fork child to pipeline commands
                int pid = fork();
                if (pid == 0) // child
                {
                        // Send output of command to pipe
                        close(fd[0]);
                        dup2(fd[1], STDOUT_FILENO);
                        close(fd[1]);

                        // Execute command
                        int is_first_command = (command_idx - 1 == 0);
                        execute_command(command_pipeline->commands[command_idx - 1], is_first_command);
                }
                else // parent
                {
                        // Grab input for command from pipe
                        close(fd[1]);
                        dup2(fd[0], STDIN_FILENO);
                        close(fd[0]);

                        // Save pid of child process
                        pids[command_idx - 1] = pid;

                        // Handle final command pipeline
                        if (command_idx == command_pipeline_length - 1)
                        {
                                execute_final_pipelined_command(command_pipeline, pids, command_input);
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

                        for (int i = 0; i < command_stack->top; i++)
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
                        (command_pipeline->commands_length == 1)
                                ? execute_single_command(command_pipeline->commands[0], command_input)
                                : execute_pipelined_command(command_pipeline, command_input);
                }
                else // parent process
                {
                        // Wait for child process to complete
                        wait(&retval);

                        // Stop execution if child did not terminate normally
                        if (!WIFEXITED(retval))
                        {
                                exit(1);
                        }
                }

        }

        return EXIT_SUCCESS;
}