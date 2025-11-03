//gcc -Wall fibr.c -o fibr
//PREPROCESSORS
#include <stdio.h> //printf
#include <string.h>           
#include <stdlib.h> //macros

//CONSTANTS
const int CHAR_SIZE = 4;
const int EUNKNOWN = 0;
const int ENOARGS = 1;
const int EBADCALL = 2;
const int EOUTRANGE = 3;




//PROTOTYPES
unsigned long fib(unsigned long);
void errorLIST(int code);
char* dynCopyFrom(char*src);

unsigned long fib(unsigned long num)
{
    if (num <= 1)
        return num;
    return fib(num-1) + fib(num-2);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        errorLIST(ENOARGS);

    unsigned long num = 0;

    num = atol(argv[1]);

    if (num > 93 || num < 1)
        errorLIST(EOUTRANGE);
    printf("%6d: %lu\n", 0, 0ul);
    printf("%6d: %lu\n", 1, 1ul);

    for (unsigned long i = 2; i <= num; i++)
        printf("%6ld: %lu\n",i,fib(i));

    return EXIT_SUCCESS;
}

void errorLIST(int code)
{
    char* msg = NULL;
    switch (code)
    {
        case(0):
            msg = dynCopyFrom("UNKNOWN ERROR OCCURED\n");
            break;
        case (1):
            msg = dynCopyFrom("No arguments provided\n");
            break;
        case (2):
            msg = dynCopyFrom("Bad Function Call\n");
            break;
        case (3):
            msg = dynCopyFrom("Enter a number between 1 & 93\n");
    }
    printf("%s\n",msg);
    free(msg);
    exit(EXIT_FAILURE);
}

char* dynCopyFrom(char*src)
{
    if (src == NULL)
        errorLIST(EBADCALL);
    int length = strlen(src) + 1;
    char* hold = (char*)malloc(length * CHAR_SIZE);
    return strcpy(hold,src);
}
