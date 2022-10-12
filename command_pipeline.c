#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "command_pipeline.h"
#include "string_utilities.h"



int validate_command_pipeline(const CommandPipeline* command_pipeline)
{
    // Check for too many arguments for ls
    for (int i=0; i<command_pipeline->commands_length; i++)
    {
        Command* command = command_pipeline->commands[i];

        if (!strcmp(command->cmd, "ls"))
        {
            int number_directories = 0;
            for (int j=0; j<command->args_len; j++)
            {
                char* argument = command->args[j];
                assert(strlen(argument) > 0);
                if (argument[0] != '-')
                {
                    number_directories++;
                }
            }
            if (number_directories > 1)
            {
                fprintf(stderr, "Error: too many process arguments\n");
                return 1;
            }
        }
    }

    const char* output_file = command_pipeline->output_file;
    if (output_file != NULL)
    {
        // Check for misplaced output redirection
        for (unsigned i=0; i<strlen(output_file); i++)
        {
            if (output_file[i] == ' ')
            {
                fprintf(stderr, "Error: mislocated output redirection\n");
                return 1;
            }
        }

        // Check for invalid output file
        int fd = open(output_file, O_TRUNC | O_WRONLY | O_CREAT, 0666);
        close(fd);
        if (fd == -1)
        {
            fprintf(stderr, "Error: cannot open output file\n");
            return 1;
        }
    }

    return 0;
}

CommandPipeline* create_command_pipeline(const char* command_string)
{
    const char* FILE_SEPARATOR = ">";
    const char* PIPE_SEPARATOR = "|";

    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));

    char* full_command_string = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    strcpy(full_command_string, command_string);
    full_command_string = strip_whitespace(full_command_string);

    if (strlen(full_command_string) == 0)
    {
        return NULL;
    }

    int is_missing_command =
        full_command_string[0] == '>' ||
        full_command_string[0] == '|' ||
        full_command_string[strlen(full_command_string)-1] == '|';
    if (is_missing_command)
    {
        fprintf(stderr, "Error: missing command\n");
        return NULL;
    }

    int is_missing_output_file = full_command_string[strlen(full_command_string)-1] == '>';
    if (is_missing_output_file)
    {
        fprintf(stderr, "Error: no output file\n");
        return NULL;
    }

    char *token = strtok(full_command_string, FILE_SEPARATOR);
    char **seperated_command_string = (char**) malloc(2 * sizeof(char*));
    int chunks = 0;
    for (unsigned i = 0; token != NULL; i++)
    {
        seperated_command_string[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(seperated_command_string[i], token);
        seperated_command_string[i] = strip_whitespace(seperated_command_string[i]);
        token = strtok(NULL, FILE_SEPARATOR);
        chunks++;
    }

    free(full_command_string);

    command_pipeline_object->output_file = NULL;
    if (chunks == 2)
    {
        command_pipeline_object->output_file = (char*) malloc((strlen(seperated_command_string[1]) + 1) * sizeof(char));
        strcpy(command_pipeline_object->output_file, seperated_command_string[1]);
        command_pipeline_object->output_file = strip_whitespace(command_pipeline_object->output_file);
        free(seperated_command_string[1]);
    }

    char* pipe_string = (char*) malloc((strlen(seperated_command_string[0]) + 1) * sizeof(char));
    char* pipe_string2 = (char*) malloc((strlen(seperated_command_string[0]) + 1) * sizeof(char));
    strcpy(pipe_string, seperated_command_string[0]);
    strcpy(pipe_string2, seperated_command_string[0]);

    free(seperated_command_string[0]);
    free(seperated_command_string);

    // Populate commands_length
    command_pipeline_object->commands_length = 0;
    token = strtok(pipe_string, PIPE_SEPARATOR);
    while (token != NULL)
    {
        command_pipeline_object->commands_length++;
        token = strtok(NULL, PIPE_SEPARATOR);
    }

    token = strtok(pipe_string2, PIPE_SEPARATOR);
    char **pipe_commands = (char**) malloc(command_pipeline_object->commands_length * sizeof(char*));
    for (unsigned i = 0; token != NULL; i++)
    {
        pipe_commands[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(pipe_commands[i], token);
        pipe_commands[i] = strip_whitespace(pipe_commands[i]);
        token = strtok(NULL, PIPE_SEPARATOR);
    }

    free(pipe_string2);
    free(pipe_string);

    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));
    for (int i = 0; i < command_pipeline_object->commands_length; i++)
    {
        command_pipeline_object->commands[i] = create_command(pipe_commands[i]);
    }

    for (int i = 0; i < command_pipeline_object->commands_length; i++)
    {
        free(pipe_commands[i]);
    }
    free(pipe_commands);

    if (validate_command_pipeline(command_pipeline_object))
    {
        return NULL;
    }

    return command_pipeline_object;
}