#ifndef COMMAND_PIPELINE_H
#define COMMAND_PIPELINE_H

#include "command.h"

typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;

CommandPipeline* create_command_pipeline(const char* command_string);

int validate_command_pipeline(const CommandPipeline* command_pipeline);

#endif