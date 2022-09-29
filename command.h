#ifndef COMMAND_H
#define COMMAND_H

typedef struct Command {
    char* cmd;
    char** args;
    int args_len;
} Command;

void populate_command(Command* command_object, const char* command_string);

#endif