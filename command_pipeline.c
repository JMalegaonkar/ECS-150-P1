#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "command_pipeline.h"



typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;


CommandPipeline* create_command_pipeline(const char* command_string)
{
    const char* FILE_SEPARATOR = ">";
    const char* PIPE_SEPARATOR = "|";

    printf("Finished creating separators\n");
    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));
    printf("Finished creating CommandPipeline object \n");

    char* full_string = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    strcpy(full_string, command_string);

    printf("Finished copying and allocating memory for input string\n");


    char *token = strtok(full_string, FILE_SEPARATOR);   
    char **separate = (char**) malloc(2 * sizeof(char*));
    for (unsigned i=0; token != NULL; i++)
    {
        separate[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        char * tmp_string = token; 
        strip_whitespace(tmp_string);
        strcpy(separate[i],tmp_string);
        token = strtok(NULL, FILE_SEPARATOR);
    }

    printf("Finished separating output file from commands\n");
    

    command_pipeline_object->output_file = (char*) malloc((strlen(separate[1]) + 1) * sizeof(char));
    strcpy(command_pipeline_object->output_file, separate[1]);

    printf("Printing command_pipeline_object->output_file: '%s' \n", command_pipeline_object->output_file);


    char* pipe_string = (char*) malloc((strlen(separate[0]) + 1) * sizeof(char));
    char* pipe_string2 = (char*) malloc((strlen(separate[0]) + 1) * sizeof(char));
    strcpy(pipe_string, separate[0]);
    strcpy(pipe_string2, separate[0]);

    printf("Finished copying and allocating memory for pipe string\n");


    // Populate commands_length
    command_pipeline_object->commands_length = -1;
    char *token1 = strtok(pipe_string, PIPE_SEPARATOR);
    while (token1 != NULL) 
    {
        command_pipeline_object->commands_length++;
        token1 = strtok(NULL, PIPE_SEPARATOR);
    }
    printf(" ^^^^ %s \n", pipe_string);
    printf(" @@@@ %s \n", pipe_string2);
    printf("Finished finding command_pipeline_object->commands_length: '%d' \n", command_pipeline_object->commands_length);
    

    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));

    printf("Finished allocating memory for command_pipeline_object->commands object\n");

    printf("%s\n", pipe_string2);
    char *token2 = strtok(pipe_string2, PIPE_SEPARATOR);
    char * tmp_string; 
    for (unsigned i=0; token2 != NULL; i++)
    {
        // printf("%s\n", pipe_string2);
        tmp_string = token2;
        strip_whitespace(tmp_string);
        printf("%s\n", tmp_string);
        Command current_command = *create_command(tmp_string);
        printf("%s \n", current_command.cmd);
        printf("%d \n", current_command.args_len);
        command_pipeline_object->commands[i] = (Command*) malloc(sizeof(current_command) + 1);
        memcpy(&command_pipeline_object->commands[i], &current_command, sizeof(current_command) + 1);

        token2 = strtok(NULL, PIPE_SEPARATOR);
    }

    free(full_string);

    return command_pipeline_object;
}
