#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "command.h"

#define CMDLINE_MAX 512

int main(void)
{
        char cmd[CMDLINE_MAX];

        while (1) 
        {
                char *nl;
                int retval;

                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) 
                {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                // make last char NULL
                nl = strchr(cmd, '\n');
                if (nl) 
                {
                        *nl = '\0';
                }

                /* Builtin command */
                //Exit case
                if (!strcmp(cmd, "exit")) 
                {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
                
                Command x = { .cmd = NULL, .args = NULL };
                populate_command(x, cmd);
                printf("command is [%s]\n", x.cmd);
                printf("arg_len is %d\n", x.args_len);
                for (int i=0; i<x.args_len; i++) {
                        printf("args[%d] = [%s]", i, x.args[i]);
                }


                // Complete Child Process First
                if (fork() == 0) 
                {
                        // child process
                        char *argv[2] = { cmd, NULL };
                        execvp(cmd, argv);
                        perror("execvp");
                        exit(1);
                }
                else
                {
                        // parent process
                        wait(&retval);
                        if (WIFEXITED(retval))
                        {
                                fprintf(stderr, "+ completed '%s' [%d]\n",
                                cmd, retval);
                        }
                        else
                        {
                                fprintf(stderr, "Child did not terminate with exit\n");
                        }

                }
        }

        return EXIT_SUCCESS;
}