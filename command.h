#ifndef COMMAND_H
#define COMMAND_H

typedef struct Command
{
    char* cmd;
    char* input_file;
    char** args;
    int args_len;
    int expects_input_file;
} Command;

Command* create_command(const char* command_string);

#endif