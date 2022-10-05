#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "command.h"
#include "command_pipeline.h"

#define CMDLINE_MAX 512

int main(void)
{
        char command_input[CMDLINE_MAX];
        char cwd[PATH_MAX];

        while (1) 
        {
                char *nl;
                int retval;

                /* Print prompt */
                fprintf(stderr, "sshell@ucd$ ");
                fflush(stdout);

                /* Get command line */
                fgets(command_input, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) 
                {
                        printf("%s", command_input);
                        fflush(stdout);
                }

                /* Replace trailing newline from command line with null terminator */
                nl = strchr(command_input, '\n');
                if (nl) 
                {
                        *nl = '\0';
                }

                /* Exit Condition */
                if (!strcmp(command_input, "exit")) 
                {
                        fprintf(stderr, "Bye...\n+ completed '%s' [0]\n", command_input);
                        break;
                }

                if (!strcmp(command_input, "pwd")) 
                {
                        getcwd(cwd, sizeof(cwd) * sizeof(char));
                        printf("%s\n", cwd);
                        continue;
                }
                
                // Parses command_input to create Command object
                CommandPipeline command_pipeline = *create_command_pipeline(command_input);

                // hardcoding to only handle 1st command for now
                // currently ignoring other commands in pipeline for now
                Command command = *command_pipeline.commands[0];

                // Complete Child Process First
                if (fork() == 0) 
                {
                        // child process

                        // Handle output redirection
                        if (command_pipeline.output_file != NULL)
                        {
                                int fd = open(command_pipeline.output_file, O_TRUNC | O_WRONLY | O_CREAT, 0666);
                                dup2(fd, STDOUT_FILENO);
                        }

                        // Populate argv for exec
                        char *argv[command.args_len + 2];
                        argv[0] = command.cmd;
                        for (int i=0; i<command.args_len; i++) 
                        {
                                argv[i+1] = command.args[i];
                        }
                        argv[command.args_len + 1] = NULL;

                        execvp(command.cmd, argv);
                        perror("execvp");
                        exit(1);
                }       
                else
                {
                        // parent process
                        wait(&retval);
                        if (!strcmp(command.cmd, "cd") && command.args_len == 1)
                        {
                                chdir(command.args[0]);
                        }

                        if (WIFEXITED(retval))
                        {
                                fprintf(stderr, "+ completed '%s' [%d]\n", command_input, retval);
                        }
                        else
                        {
                                fprintf(stderr, "Child did not terminate with exit\n");
                        }

                }      

        }

        return EXIT_SUCCESS;
}