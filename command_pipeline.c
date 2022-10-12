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
                return 0;
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
                return 0;
            }
        }

        // Check for invalid output file
        int fd = open(output_file, O_TRUNC | O_WRONLY | O_CREAT, 0666);
        close(fd);
        if (fd == -1)
        {
            fprintf(stderr, "Error: cannot open output file\n");
            return 0;
        }
    }

    return 1;
}

int validate_raw_command_string(char* command_string)
{
    if (strlen(command_string) == 0)
    {
        return 0;
    }

    int is_missing_command =
        command_string[0] == '>' ||
        command_string[0] == '|' ||
        command_string[strlen(command_string)-1] == '|';
    if (is_missing_command)
    {
        fprintf(stderr, "Error: missing command\n");
        return 0;
    }

    int is_missing_output_file = command_string[strlen(command_string)-1] == '>';
    if (is_missing_output_file)
    {
        fprintf(stderr, "Error: no output file\n");
        return 0;
    }

    return 1;
}

void parse_output_redirection(
    char* stripped_full_command_string,
    char** seperated_command_string,
    char** stripped_seperated_command_string,
    CommandPipeline* command_pipeline_object,
    const char* seperator)
{
    char* token = strtok(stripped_full_command_string, seperator);
    int chunks = 0;
    for (unsigned i = 0; token != NULL; i++)
    {
        seperated_command_string[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(seperated_command_string[i], token);
        stripped_seperated_command_string[i] = strip_whitespace(seperated_command_string[i]);
        token = strtok(NULL, seperator);
        chunks++;
    }

    char* output_file = stripped_seperated_command_string[1];
    command_pipeline_object->output_file = NULL;
    if (chunks == 2)
    {
        command_pipeline_object->output_file = (char*) malloc((strlen(output_file) + 1) * sizeof(char));
        strcpy(command_pipeline_object->output_file, output_file);
        command_pipeline_object->output_file = strip_whitespace(command_pipeline_object->output_file);
        free(seperated_command_string[1]);
    }
}

void parse_pipelined_commands(
    char* main_command_string,
    CommandPipeline* command_pipeline_object,
    const char* seperator)
{
    char* pipe_string = (char*) malloc((strlen(main_command_string) + 1) * sizeof(char));
    strcpy(pipe_string, main_command_string);

    // Populate commands_length
    command_pipeline_object->commands_length = 0;
    char* token = strtok(pipe_string, seperator);
    while (token != NULL)
    {
        command_pipeline_object->commands_length++;
        token = strtok(NULL, seperator);
    }

    // Populate command strings
    strcpy(pipe_string, main_command_string);
    token = strtok(pipe_string, seperator);
    char* pipe_commands[command_pipeline_object->commands_length];
    char* stripped_pipe_commands[command_pipeline_object->commands_length];
    for (unsigned i = 0; token != NULL; i++)
    {
        pipe_commands[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(pipe_commands[i], token);
        stripped_pipe_commands[i] = strip_whitespace(pipe_commands[i]);
        token = strtok(NULL, seperator);
    }
    free(pipe_string);

    // Populate command objects
    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));
    for (int i = 0; i < command_pipeline_object->commands_length; i++)
    {
        command_pipeline_object->commands[i] = create_command(stripped_pipe_commands[i]);
        free(pipe_commands[i]);
    }
}

CommandPipeline* create_command_pipeline(const char* command_string)
{
    const char* FILE_SEPARATOR = ">";
    const char* PIPE_SEPARATOR = "|";

    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));

    char* full_command_string = (char*) malloc((strlen(command_string) + 1) * sizeof(char));
    strcpy(full_command_string, command_string);
    char* stripped_full_command_string = strip_whitespace(full_command_string);

    // Check for "missing command" or "no output file"
    if (!validate_raw_command_string(stripped_full_command_string))
    {
        free(full_command_string);
        return NULL;
    }

    // Parse output redirection
    char* seperated_command_string[2] = { NULL, NULL };
    char* stripped_seperated_command_string[2] = { NULL, NULL };
    parse_output_redirection(
        stripped_full_command_string,
        seperated_command_string,
        stripped_seperated_command_string,
        command_pipeline_object,
        FILE_SEPARATOR);
    free(full_command_string);

    // Parse pipelined commands
    char* main_command_string = stripped_seperated_command_string[0];
    parse_pipelined_commands(
        main_command_string,
        command_pipeline_object,
        PIPE_SEPARATOR);
    free(seperated_command_string[0]);

    // Check for "too many ls arguments", "misplaced output redirection", or "invalid output file"
    if (!validate_command_pipeline(command_pipeline_object))
    {
        return NULL;
    }

    return command_pipeline_object;
}