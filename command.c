#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "command.h"
#include "string_utilities.h"

Command* create_command(const char* command_string)
{
    const char* INPUT_FILE_SEPERATOR = "<";
    const char* WHITESPACE_SEPARATOR = " ";

    Command* command_object = (Command*) malloc(sizeof(Command));
    command_object->expects_input_file = strchr(command_string, '<') != NULL;

    char* full_command_string = (char*) malloc((strlen(command_string) + 1) * sizeof(char)); // used to seperate main command and input file
    strcpy(full_command_string, command_string);

    char* token = strtok(full_command_string, INPUT_FILE_SEPERATOR);
    assert(token != NULL);

    // Extract main command from full command
    char* main_command_string = (char*) malloc((strlen(token) + 1) * sizeof(char));
    strcpy(main_command_string, token);
    main_command_string = strip_whitespace(main_command_string);
    token = strtok(NULL, INPUT_FILE_SEPERATOR);

    // Extract input file from full command (if it exists)
    command_object->input_file = NULL;
    if (token != NULL)
    {
        command_object->input_file = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(command_object->input_file, token);
        command_object->input_file = strip_whitespace(command_object->input_file);
    }

    char* command_string1 = (char*) malloc((strlen(main_command_string) + 1) * sizeof(char)); // used to get length
    char* command_string2 = (char*) malloc((strlen(main_command_string) + 1) * sizeof(char)); // used to get cmd & args

    strcpy(command_string1, main_command_string);
    strcpy(command_string2, main_command_string);

    // Populate args_len
    command_object->args_len = -1;
    token = strtok(command_string1, WHITESPACE_SEPARATOR);
    while (token != NULL) 
    {
        command_object->args_len++;
        token = strtok(NULL, WHITESPACE_SEPARATOR);
    }
    assert(command_object->args_len >= 0);

    // Populate cmd
    token = strtok(command_string2, WHITESPACE_SEPARATOR);
    command_object->cmd = (char*) malloc((strlen(token) + 1) * sizeof(char));
    strcpy(command_object->cmd, token);

    // Populate args
    command_object->args = (char**) malloc(command_object->args_len * sizeof(char*));
    token = strtok(NULL, WHITESPACE_SEPARATOR);
    for (unsigned i=0; token != NULL; i++)
    {
        command_object->args[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(command_object->args[i], token);
        token = strtok(NULL, WHITESPACE_SEPARATOR);
    }

    free(full_command_string);
    free(command_string1);
    free(command_string2);

    return command_object;
}
