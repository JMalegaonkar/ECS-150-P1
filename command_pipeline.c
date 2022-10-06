#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "command_pipeline.h"

char* strip_whitespace(char *string)
{
    char *end;

    // strip leading white space 
    while(isspace((unsigned char)*string))
    {
        string++;
    }

    if(*string == 0)
    {
        return string;
    }

    // strip trailing white space
    end = string + strlen(string) - 1;
    while(end > string && isspace((unsigned char)*end)) 
    {
        end--;
    }
    end[1] = '\0';

    return string;
}

CommandPipeline* create_command_pipeline(const char* command_string)
{
    // tokens for parsing
    const char* O_FILE_SEPARATOR = ">";
    const char* I_FILE_SEPARATOR = "<";
    const char* PIPE_SEPARATOR = "|";


    // create new Command Pipeline object
    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));

    char* full_command_string1 = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    char* full_command_string2 = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    strcpy(full_command_string1, command_string);
    strcpy(full_command_string2, command_string);

    bool in = false;
    bool out = false;

    char* token = strtok(full_command_string1, O_FILE_SEPARATOR);   
    char** o_seperated_command_string = (char**) malloc(2 * sizeof(char*));
    int o_seperated_chunks = 0;

    // Find and separate redirection to output file if redirection exists
    for (unsigned i=0; token != NULL; i++)
    {
        o_seperated_command_string[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(o_seperated_command_string[i], token);
        o_seperated_command_string[i] = strip_whitespace(o_seperated_command_string[i]);
        token = strtok(NULL, O_FILE_SEPARATOR);
        if (token) {out = true;} 
        o_seperated_chunks++;
    }

    // if redirection exists allocate memory for outputfile in Command Pipeline object else NULL
    command_pipeline_object->output_file = NULL;
    if (o_seperated_chunks == 2 && out) 
    {
        command_pipeline_object->output_file = (char*) malloc((strlen(o_seperated_command_string[1]) + 1) * sizeof(char));
        strcpy(command_pipeline_object->output_file, o_seperated_command_string[1]);
    } 

    token = strtok(full_command_string2, I_FILE_SEPARATOR);   
    char** i_seperated_command_string = (char**) malloc(2 * sizeof(char*));
    int i_seperated_chunks = 0;

    // Find and separate redirection to output file if redirection exists
    for (unsigned i=0; token != NULL; i++)
    {
        i_seperated_command_string[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(i_seperated_command_string[i], token);
        i_seperated_command_string[i] = strip_whitespace(i_seperated_command_string[i]);
        token = strtok(NULL, I_FILE_SEPARATOR);
        if (token) {in = true;} 
        i_seperated_chunks++;
    }

    // if redirection exists allocate memory for outputfile in Command Pipeline object else NULL
    command_pipeline_object->input_file = NULL;
    if (i_seperated_chunks == 2 && in) 
    {
        command_pipeline_object->input_file = (char*) malloc((strlen(i_seperated_command_string[1]) + 1) * sizeof(char));
        strcpy(command_pipeline_object->input_file, i_seperated_command_string[1]);
    } 


    // copy and allocate memory for pipe string
    char* pipe_string1 = (char*) malloc((strlen(o_seperated_command_string[0]) + 1) * sizeof(char));
    char* pipe_string2 = (char*) malloc((strlen(o_seperated_command_string[0]) + 1) * sizeof(char));
    strcpy(pipe_string1, o_seperated_command_string[0]);
    strcpy(pipe_string2, o_seperated_command_string[0]);

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
        pipe_commands[i] = strip_whitespace(pipe_commands[i]);
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
    free(full_command_string1);
    free(full_command_string2);
    free(i_seperated_command_string);
    free(o_seperated_command_string);
    free(pipe_string1);
    free(pipe_string2);
    free(pipe_commands);

    return command_pipeline_object;
}



