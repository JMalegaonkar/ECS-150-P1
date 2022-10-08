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
    ret_stack->stack = (char**) malloc(sizeof(char*) * size);

    return ret_stack;
}

int is_empty(CommandStack* s) 
{
    if (s == NULL) 
    {
        fprintf(stderr, "Cannot work with NULL stack.\n");
    }
    return s->top < 0;
}

void double_size(CommandStack* s)
{
    s->max_size = 2 * s->max_size;
    char** doubled_stack = (char**) malloc(sizeof(char) * s->max_size);
    for (int i = 0; i < s->top ; i ++)
    {
        doubled_stack[i] = (char*) malloc((strlen(s->stack[i]) + 1) * sizeof(char));
        strcpy(doubled_stack[i],s->stack[i]);
    }
    free(s->stack);
    s->stack = doubled_stack;
}

void push(CommandStack* s, char* directory)
{
    if(s->top >= (s->max_size - 1)/2) double_size(s);

    s->top++;
    s->stack[s->top] = (char*) malloc((strlen(directory) + 1) * sizeof(char));
    strcpy(s->stack[s->top], directory);
}

void pop(CommandStack* s) 
{
    if (is_empty(s)) fprintf(stderr, "Error: directory stack empty\n");

    s->top--;
}

char* top(CommandStack* s) 
{
    if (is_empty(s)) fprintf(stderr, "Error: directory stack empty\n");

    return s->stack[s->top];
}

void get_commands(CommandStack* s, char* cwd)
{
    if (is_empty(s))
    {   
        fprintf(stderr, "%s\n", cwd);
    }
    else
    {
        printf("%s\n", cwd);
    }

    for (int i = s->top; i >= 0; i--)
    {
        printf("%s\n", s->stack[i]);
    }
}








