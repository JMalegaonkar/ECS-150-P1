#ifndef COMMAND_STACK
#define COMMAND_STACK



// https://d-michail.github.io/assets/teaching/data-structures/015_StackImplementation.en.pdf
typedef struct CommandStack
{
   char** directory_stack;
   int max_size;
   int top;
} CommandStack;

CommandStack* create_stack(int size);

int is_empty(CommandStack* command_stack);

void double_size(CommandStack* command_stack);

void push(CommandStack* command_stack, char* directory);

void pop(CommandStack* command_stack);

char* top(CommandStack* command_stack);

void get_commands(CommandStack* command_stack, char* cwd);

#endif