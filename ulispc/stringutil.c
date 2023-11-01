#include "stringutil.h"
#include <string.h>
#include <ctype.h>

/*
 * Modify the given string to remove whitespace from the beginning and end.
 */
char*
strip_whitespace(char* str)
{
    char* end;

    // Trim leading space
    while (isspace((unsigned char) *str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end))
        end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}