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
        begin++;

    while ((end >= begin) && isspace((unsigned char) string[end]))
        end--;

    // Shift all characters back to the start of the string array.
    int i;
    for (i = begin; i <= end; i++)
        string[i - begin] = string[i];

}

CommandPipeline* create_command_pipeline(const char* command_string)
{
    // tokens for parsing
    const char* FILE_SEPARATOR = ">";
    const char* PIPE_SEPARATOR = "|";


    // create new Command Pipeline object
    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));

    char* full_string = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    strcpy(full_string, command_string);


    char *token = strtok(full_string, FILE_SEPARATOR);   
    char **separate = (char**) malloc(2 * sizeof(char*));
    int seperated_chunks = 0;

    // Find and separate redirection to output file if redirection exists
    for (unsigned i=0; token != NULL; i++)
    {
        char * clean_string = token; 
        strip_whitespace(clean_string);
        separate[i] = (char*) malloc((strlen(clean_string) + 1) * sizeof(char));
        strcpy(separate[i],clean_string);
        token = strtok(NULL, FILE_SEPARATOR);
        seperated_chunks += 1;
    }

    // if redirection exists allocate memory for outputfile in Command Pipeline object else NULL
    if (seperated_chunks == 2) 
    {
        command_pipeline_object->output_file = (char*) malloc((strlen(separate[1]) + 1) * sizeof(char));
        strcpy(command_pipeline_object->output_file, separate[1]);
    } 
    else 
    {
        command_pipeline_object->output_file = NULL;
    }


    // copy and allocate memory for pipe string
    char* pipe_string1 = (char*) malloc((strlen(separate[0]) + 1) * sizeof(char));
    char* pipe_string2 = (char*) malloc((strlen(separate[0]) + 1) * sizeof(char));
    strcpy(pipe_string, separate[0]);
    strcpy(pipe_string2, separate[0]);


    // Populate commands_length
    command_pipeline_object->commands_length = 0;
    char *token1 = strtok(pipe_string1, PIPE_SEPARATOR);
    while (token1 != NULL) 
    {
        command_pipeline_object->commands_length++;
        token1 = strtok(NULL, PIPE_SEPARATOR);
    }

    // create list of commands in the pipeline
    char *token2 = strtok(pipe_string2, PIPE_SEPARATOR);
    char **pipe_commands = (char**) malloc(command_pipeline_object->commands_length * sizeof(char*));
    for (unsigned i = 0; token2 != NULL; i++)
    {
        // printf("%s\n", pipe_string2);
        char * tmp_string2 = token2; 
        strip_whitespace(tmp_string2);
        pipe_commands[i] = (char*) malloc((strlen(tmp_string2) + 1) * sizeof(char));
        strcpy(pipe_commands[i],tmp_string2);
        token2 = strtok(NULL, PIPE_SEPARATOR);
    }

    
    // Create commands objects for each command in pipline and populate commands
    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));
    for (int k = 0; k < command_pipeline_object->commands_length; k ++ )
    {
        Command current_command = *create_command(pipe_commands[k]);
        command_pipeline_object->commands[k] = (Command*) malloc(sizeof(current_command) + 1);
        memcpy(command_pipeline_object->commands[k], &current_command, sizeof(current_command) + 1);
    }

    // free all allocated memory
    free(full_string);
    free(separate);
    free(pipe_string1);
    free(pipe_string2);
    free(pipe_commands);

    return command_pipeline_object;
}



