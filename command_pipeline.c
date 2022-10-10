#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "command_pipeline.h"
#include "string_utilities.h"



char** separate_file(CommandPipeline* command_pipeline_object, char* full_command_string, const char* FILE_SEPARATOR)
{

    char* token = strtok(full_command_string, FILE_SEPARATOR);   
    char** seperated_command_string = (char**) malloc(2 * sizeof(char*));

    command_pipeline_object->seperated_chunks = 0;

    // Find and separate redirection to output file if redirection exists
    for (unsigned i=0; token != NULL; i++)
    {
        seperated_command_string[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(seperated_command_string[i], token);
        seperated_command_string[i] = strip_whitespace(seperated_command_string[i]);
        token = strtok(NULL, FILE_SEPARATOR);
        command_pipeline_object->seperated_chunks++;
    }
    free(full_command_string);

    return seperated_command_string;
}

void populate_output_file(CommandPipeline* command_pipeline_object, char** seperated_command_string)
{
    // if redirection exists allocate memory for outputfile in Command Pipeline object else NULL
    command_pipeline_object->output_file = NULL;
    if (command_pipeline_object->seperated_chunks == 2) 
    {
        command_pipeline_object->output_file = (char*) malloc((strlen(seperated_command_string[1]) + 1) * sizeof(char));
        strcpy(command_pipeline_object->output_file, seperated_command_string[1]);
    } 
}

char** separate_commands(CommandPipeline* command_pipeline_object, char** seperated_command_string, const char* PIPE_SEPARATOR)
{
    char* pipe_string1 = (char*) malloc((strlen(seperated_command_string[0]) + 1) * sizeof(char));
    char* pipe_string2 = (char*) malloc((strlen(seperated_command_string[0]) + 1) * sizeof(char));
    strcpy(pipe_string1, seperated_command_string[0]);
    strcpy(pipe_string2, seperated_command_string[0]);

    // Populate commands_length
    command_pipeline_object->commands_length = 0;
    char* token = strtok(pipe_string1, PIPE_SEPARATOR);
    while (token != NULL) 
    {
        command_pipeline_object->commands_length++;
        token = strtok(NULL, PIPE_SEPARATOR);
    }

    // create list of commands in the pipeline
    token = strtok(pipe_string2, PIPE_SEPARATOR);
    char** seperated_pipe_commands = (char**) malloc(command_pipeline_object->commands_length * sizeof(char*));
    for (unsigned i = 0; token != NULL; i++)
    {
        seperated_pipe_commands[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(seperated_pipe_commands[i], token);
        seperated_pipe_commands[i] = strip_whitespace(seperated_pipe_commands[i]);
        token = strtok(NULL, PIPE_SEPARATOR);
    } 
    free(pipe_string1);
    free(pipe_string2);  

    return seperated_pipe_commands;
}

void populate_commands(CommandPipeline* command_pipeline_object, char** seperated_pipe_commands)
{
    // Create commands objects for each command in pipline and populate commands
    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));
    for (int i = 0; i < command_pipeline_object->commands_length; i++)
    {
        Command current_command = *create_command(seperated_pipe_commands[i]);
        command_pipeline_object->commands[i] = (Command*) malloc(sizeof(current_command) + 1);
        memcpy(command_pipeline_object->commands[i], &current_command, sizeof(current_command) + 1);
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

    char** seperated_command_string = separate_file(command_pipeline_object, full_command_string, FILE_SEPARATOR);

    populate_output_file(command_pipeline_object, seperated_command_string);

    char** seperated_pipe_commands = separate_commands(command_pipeline_object,seperated_command_string, PIPE_SEPARATOR);

    populate_commands(command_pipeline_object, seperated_pipe_commands);

    free(seperated_command_string);
    free(seperated_pipe_commands);

    return command_pipeline_object;
}



