#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "command_pipeline.h"

void strip_whitespace(char *string)
{
    int begin = 0;
    int end = strlen(string) - 1;

    while (isspace((unsigned char) string[begin]))
    {
        begin++;
    }

    while ((end >= begin) && isspace((unsigned char) string[end]))
    {
        end--;
    }

    // Shift all characters back to the start of the string array.
    for (int i = begin; i <= end; i++)
    {
        string[i - begin] = string[i];
    }
}

CommandPipeline* create_command_pipeline(const char* command_string)
{
    // tokens for parsing
    const char* FILE_SEPARATOR = ">";
    const char* PIPE_SEPARATOR = "|";


    // create new Command Pipeline object
    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));

    char* full_command_string = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    strcpy(full_command_string, command_string);

    char* token = strtok(full_command_string, FILE_SEPARATOR);   
    char** seperated_command_string = (char**) malloc(2 * sizeof(char*));
    int seperated_chunks = 0;

    // Find and separate redirection to output file if redirection exists
    for (unsigned i=0; token != NULL; i++)
    {
        seperated_command_string[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(seperated_command_string[i], token);
        strip_whitespace(seperated_command_string[i]);
        token = strtok(NULL, FILE_SEPARATOR);
        seperated_chunks++;
    }

    // if redirection exists allocate memory for outputfile in Command Pipeline object else NULL
    command_pipeline_object->output_file = NULL;
    if (seperated_chunks == 2) 
    {
        command_pipeline_object->output_file = (char*) malloc((strlen(seperated_command_string[1]) + 1) * sizeof(char));
        strcpy(command_pipeline_object->output_file, seperated_command_string[1]);
    } 

    // copy and allocate memory for pipe string
    char* pipe_string1 = (char*) malloc((strlen(seperated_command_string[0]) + 1) * sizeof(char));
    char* pipe_string2 = (char*) malloc((strlen(seperated_command_string[0]) + 1) * sizeof(char));
    strcpy(pipe_string1, seperated_command_string[0]);
    strcpy(pipe_string2, seperated_command_string[0]);

    // Populate commands_length
    command_pipeline_object->commands_length = 0;
    token = strtok(pipe_string1, PIPE_SEPARATOR);
    while (token != NULL) 
    {
        command_pipeline_object->commands_length++;
        token = strtok(NULL, PIPE_SEPARATOR);
    }

    // create list of commands in the pipeline
    token = strtok(pipe_string2, PIPE_SEPARATOR);
    char** pipe_commands = (char**) malloc(command_pipeline_object->commands_length * sizeof(char*));
    for (unsigned i = 0; token != NULL; i++)
    {
        pipe_commands[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(pipe_commands[i], token);
        strip_whitespace(pipe_commands[i]);
        token = strtok(NULL, PIPE_SEPARATOR);
    }

    
    // Create commands objects for each command in pipline and populate commands
    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));
    for (int i = 0; i < command_pipeline_object->commands_length; i++)
    {
        Command current_command = *create_command(pipe_commands[i]);
        command_pipeline_object->commands[i] = (Command*) malloc(sizeof(current_command) + 1);
        memcpy(command_pipeline_object->commands[i], &current_command, sizeof(current_command) + 1);
    }

    // free all allocated memory
    free(full_command_string);
    free(seperated_command_string);
    free(pipe_string1);
    free(pipe_string2);
    free(pipe_commands);

    return command_pipeline_object;
}



