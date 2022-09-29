#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct Command {
    char* cmd;
    char** args;
    int args_len;
} Command;

void populate_command(Command* command_object, const char* command_string)
{
    char* command_string1 = (char*) malloc(strlen(command_string) + 1);
    char* command_string2 = (char*) malloc(strlen(command_string) + 1);

    strcpy(command_string1, command_string);
    strcpy(command_string2, command_string);

    // Populate args_len
    command_object->args_len = -1;
    char *pch = strtok(command_string1, " ");
    while (pch != NULL) 
    {
        command_object->args_len++;
        pch = strtok(NULL, " ");
    }
    // assert(command_object->args_len > 0);

    // Populate cmd
    pch = strtok(command_string2, " ");
    command_object->cmd = (char*) malloc(strlen(pch) + 1);
    strcpy(command_object->cmd, pch);
    pch = strtok(NULL, " ");

    // Populate args
    command_object->args = (char**) malloc(command_object->args_len * sizeof(char*));
    int i = 0;
    while (pch != NULL)
    {
        command_object->args[i] = (char*) malloc(strlen(pch) + 1);
        strcpy(command_object->args[i], pch);
        pch = strtok(NULL, " ");
        i++;
    }

    free(command_string1);
    free(command_string2);
}