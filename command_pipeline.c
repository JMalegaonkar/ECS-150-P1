#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "command_pipeline.h"

CommandPipeline* create_command_pipeline(const char* command_string)
{
    const char* SEPARATOR = " ";

    CommandPipeline* command_pipeline_object = (CommandPipeline*) malloc(sizeof(CommandPipeline));

    char* command_string1 = (char*) malloc((strlen(command_string) + 1) * sizeof(char)); // used to get length
    char* command_string2 = (char*) malloc((strlen(command_string) + 1) * sizeof(char)); // used to get cmd & args

    

}