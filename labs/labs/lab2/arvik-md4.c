#include "arvik.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>


static bool extract = false;
static bool create = false;
static bool toc = false;
static bool filename = false;
static bool help = false;
static bool verbose = false;
static bool verify = false;

static char* filename_string = NULL;
static bool global_debug = false;


# define DEBUG 

#ifndef DEBUG
# define SHOW(name, var)
# define SHOWI(name, var)
# define NEWLINE()
#endif
#ifdef DEBUG
#   define SHOW(name, var) fprintf(stderr, "%s %s", name, var)
#   define SHOWI(name, var) fprintf(stderr, "%s %d", name, var)
#   define NEWLINE() fprintf(stderr, "\n");
#endif 

void displayHelp(void);

void displayHelp(void)
{
    printf("\n\
Usage: arvik-md4 -[cxtvVf:h] archive-file file...\n\
\t-c           create a new archive file\n\
\t-x           extract members from an existing archive file\n\
\t-t           show the table of contents of archive file\n\
\t-f filename  name of archive file to use\n\
\t-V           Validate the md4 values for the header and data\n\
\t-v           verbose output\n\
\t-h           show help text\n\
");
}


int main(int argc, char**argv)
{
    char c;
    FILE* IN [[maybe_unused]];
    FILE* OUT [[maybe_unused]];
    IN = stdin;
    OUT = stdout;
    for (int i = 0; i < argc; ++i)
    {
        SHOWI("arg", i);
        SHOW(": ", argv[i]);
        NEWLINE();
    }
    while ((c = getopt(argc, argv, ARVIK_OPTIONS)) != -1)
    {
        switch(c)
        {
            case 'x':
                extract = true;
                break;
            case 'c':
                create = true;
                break;
            case 't':
                toc = true;
                break;
            case 'f':
                filename = true;
                filename_string = strdup(optarg);
                break;
            case 'h':
                help = true;
                displayHelp();
                c = -1;
                break;
            case 'v':
                verbose = true;
                break;
            case 'V':
                verify = true;
                break;
        }
    }
    
if (global_debug)
{

}
if (!help)
{

}
    exit(EXIT_SUCCESS);
}
