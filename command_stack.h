#ifndef COMMAND_STACK
#define COMMAND_STACK



// https://d-michail.github.io/assets/teaching/data-structures/015_StackImplementation.en.pdf
typedef struct CommandStack
{
   char** stack;
   int max_size;
   int top;
} CommandStack;

CommandStack* create_stack(int size);

int is_empty(CommandStack* s);

void double_size(CommandStack* s);

void push(CommandStack* s, char* directory);

void pop(CommandStack* s);

char* top(CommandStack* s);

void get_commands(CommandStack* s, char* cwd);

#endif