#ifndef COMMAND_H
#define COMMAND_H

typedef struct Command {
    char* cmd;
    char** args;
} Command;

void populate_command(Command command_object, const char* command_string);

#endif