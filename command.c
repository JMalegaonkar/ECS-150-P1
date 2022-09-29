#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Command {
    char* cmd;
    char** args;
    int args_len;
} Command;

void populate_command(Command command_object, const char* command_string)
{
    //printf("%s %s\n", *command_object, command_string);
    //^dumb command to suppress warning
    printf("0\n");
    char* command_string1 = (char*) malloc(strlen(command_string) + 1);
    char* command_string2 = (char*) malloc(strlen(command_string) + 1);
    printf("-1\n");
    strcpy(command_string1, command_string);
    strcpy(command_string2, command_string);

    printf("1\n");

    // Populate args_len
    command_object.args_len = 0;
    char *pch = strtok(command_string1, " ");
    printf("2\n");
    while (pch != NULL) 
    {
        command_object.args_len++;
        pch = strtok(NULL, " ");
    }
    printf("3\n");
    // Populate cmd
    pch = strtok(command_string2, " ");
    command_object.cmd = (char*) malloc(strlen(pch) + 1);
    strcpy(command_object.cmd, pch);
    pch = strtok(NULL, " ");
    printf("4\n");
    // Populate args
    while (pch != NULL)
    {
        printf ("%s\n", pch);
        pch = strtok(NULL, " ");
    }

    free(command_string2);
    printf("%d %s\n", command_object.args_len, command_object.cmd);
    // ToDo
}