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
    return NULL;
}


//     // Populate args_len
//     command_object->args_len = -1;
//     char *token = strtok(command_string1, SEPARATOR);
//     while (token != NULL) 
//     {
//         command_object->args_len++;
//         token = strtok(NULL, SEPARATOR);
//     }
//     assert(command_object->args_len >= 0);

//     // Populate cmd
//     token = strtok(command_string2, SEPARATOR);
//     command_object->cmd = (char*) malloc((strlen(token) + 1) * sizeof(char));
//     strcpy(command_object->cmd, token);

//     // Populate args
//     command_object->args = (char**) malloc(command_object->args_len * sizeof(char*));
//     token = strtok(NULL, SEPARATOR);
//     for (unsigned i=0; token != NULL; i++)
//     {
//         command_object->args[i] = (char*) malloc((strlen(token) + 1) * sizeof(char));
//         strcpy(command_object->args[i], token);
//         token = strtok(NULL, SEPARATOR);
//     }

//     free(command_string1);
//     free(command_string2);

//     return command_object;

// }