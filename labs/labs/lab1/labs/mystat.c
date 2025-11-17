#include "generalFuncs.h"

typedef enum {
    READ,
    WRITE,
    EXECUTE
} permissions;

char setLetter(bool isALLOWED, permissions p)
{
    char expected;
    if (!isALLOWED) expected = '-';
    else if (p == READ) expected = 'r';
    else if (p == WRITE) expected = 'w';
    else if (p == EXECUTE) expected = 'x';
    return expected;
}

char* perms(mode_t list)
{
    char octal[10];
    sprintf(octal, "%.9o", list);
    
}