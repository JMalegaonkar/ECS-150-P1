#ifndef COMMAND_PIPELINE_H
#define COMMAND_PIPELINE_H

#include "command.h"

typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
    int seperated_chunks;
} CommandPipeline;

CommandPipeline* create_command_pipeline(const char* command_string);

#endif