#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


typedef struct Command 
{
    char* cmd;
    char** args;
    int args_len;
} Command;

Command* create_command(const char* command_string)
{
    const char* SEPARATOR = " ";

    Command* command_object = (Command*) malloc(sizeof(Command));

    char* command_string1 = (char*) malloc((strlen(command_string) + 1) * sizeof(char)); // used to get length
    char* command_string2 = (char*) malloc((strlen(command_string) + 1) * sizeof(char)); // used to get cmd & args

    strcpy(command_string1, command_string);
    strcpy(command_string2, command_string);

    // Populate args_len
    command_object->args_len = -1;
    char *token = strtok(command_string1, SEPARATOR);
    while (token != NULL) 
    {
        command_object->args_len++;
        token = strtok(NULL, SEPARATOR);
    }
    assert(command_object->args_len >= 0);

    // Populate cmd
    token = strtok(command_string2, SEPARATOR);
    command_object->cmd = (char*) malloc((strlen(token) + 1) * sizeof(char));
    strcpy(command_object->cmd, token);

    // Populate args
    command_object->args = (char**) malloc(command_object->args_len * sizeof(char*));
    token = strtok(NULL, SEPARATOR);
    for (unsigned i=0; token != NULL; i++)
    {
        command_object->args[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(command_object->args[i], token);
        token = strtok(NULL, SEPARATOR);
    }

    free(command_string1);
    free(command_string2);

    return command_object;
}


typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;

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

    string[i - begin] = '\0'; // Null terminate string.
}

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
        char * tmp_string = token; 
        strip_whitespace(tmp_string);
        separate[i] = (char*) malloc((strlen(tmp_string) + 1) * sizeof(char));
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
    command_pipeline_object->commands_length = 0;
    char *token1 = strtok(pipe_string, PIPE_SEPARATOR);
    while (token1 != NULL) 
    {
        command_pipeline_object->commands_length++;
        token1 = strtok(NULL, PIPE_SEPARATOR);
    }

    printf("Finished finding command_pipeline_object->commands_length: '%d' \n", command_pipeline_object->commands_length);
    

    printf("%s\n", pipe_string2);
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

    printf("Finished creating list of commands \n");


    command_pipeline_object->commands = (Command**) malloc(command_pipeline_object->commands_length * sizeof(Command*));
    printf("Finished allocating memory for command_pipeline_object->commands object\n");
    for (int k = 0; k < command_pipeline_object->commands_length; k ++ )
    {
        Command current_command = *create_command(pipe_commands[k]);
        printf("%s \n", current_command.cmd);
        printf("%d \n", current_command.args_len);
        command_pipeline_object->commands[k] = (Command*) malloc(sizeof(current_command) + 1);
        memcpy(command_pipeline_object->commands[k], &current_command, sizeof(current_command) + 1);
    }
    printf("Finished creating Command objects and copying them into pipleine object\n");

    free(full_string);
    free(pipe_commands);

    return command_pipeline_object;
}


int main()
{
    char* command_string = "du -sh * | sort -h -r | head -3>output_file";
    CommandPipeline x  = *create_command_pipeline(command_string);

    printf("%s\n", x.output_file);
    printf("%d\n", x.commands_length);
    for (int k = 0; k < x.commands_length; k ++ )
    {
        printf("%s\n", x.commands[k]->cmd);
        printf("%d\n", x.commands[k]->args_len);
    }








}