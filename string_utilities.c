#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "string_utilities.h"

char* strip_whitespace(char *string)
{
    char *end;

    // strip leading white space
    while(isspace((unsigned char)*string))
    {
        string++;
    }

    if(*string == 0)
    {
        return string;
    }

    // strip trailing white space
    end = string + strlen(string) - 1;
    while(end > string && isspace((unsigned char)*end))
    {
        end--;
    }
    end[1] = '\0';

    return string;
}