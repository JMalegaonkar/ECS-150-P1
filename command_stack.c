#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "command_stack.h"

CommandStack* create_stack(int size)
{
    CommandStack* ret_stack = (CommandStack*) malloc(sizeof(CommandStack));

    ret_stack->max_size = size;
    ret_stack->top = -1;
    ret_stack->directory_stack = (char**) malloc(sizeof(char*) * size);

    return ret_stack;
}

int is_empty(CommandStack* command_stack)
{
    if (command_stack == NULL)
    {
        fprintf(stderr, "Cannot work with NULL stack.\n");
    }
    return command_stack->top < 0;
}

void double_size(CommandStack* command_stack)
{
    command_stack->max_size = 2 * command_stack->max_size;
    char** doubled_stack = (char**) malloc(sizeof(char) * command_stack->max_size);
    for (int i = 0; i < command_stack->top ; i ++)
    {
        doubled_stack[i] = (char*) malloc((strlen(command_stack->directory_stack[i]) + 1) * sizeof(char));
        strcpy(doubled_stack[i],command_stack->directory_stack[i]);
        free(command_stack->directory_stack[i]);
    }

    free(command_stack->directory_stack);
    command_stack->directory_stack = doubled_stack;
}

void push(CommandStack* command_stack, char* directory)
{
    if(command_stack->top >= (command_stack->max_size - 1)/2)
    {
        double_size(command_stack);
    }

    command_stack->top++;
    command_stack->directory_stack[command_stack->top] = (char*) malloc((strlen(directory) + 1) * sizeof(char));
    strcpy(command_stack->directory_stack[command_stack->top], directory);
}

void pop(CommandStack* command_stack)
{
    if (is_empty(command_stack))
    {
        fprintf(stderr, "Error: directory stack empty\n");
    }

    if (command_stack->top > -1)
    {
        free(command_stack->directory_stack[command_stack->top]);
    }

    command_stack->top--;
}

char* top(CommandStack* command_stack)
{
    if (is_empty(command_stack))
    {
        fprintf(stderr, "Error: directory stack empty\n");
    }

    return command_stack->directory_stack[command_stack->top];
}

void get_commands(CommandStack* command_stack, char* cwd)
{
    if (is_empty(command_stack))
    {
        fprintf(stderr, "%s\n", cwd);
    }
    else
    {
        printf("%s\n", cwd);
    }

    for (int i = command_stack->top; i >= 0; i--)
    {
        printf("%s\n", command_stack->directory_stack[i]);
    }
}








