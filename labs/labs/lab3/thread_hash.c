#include "thread_hash.h"
#include <stdio.h>

options_t option_init = {.inputExists = false, .output = false, .directory = false, .threads = 1, .verbose = false, .help = true, .nice = false};

void be_nice()
{

}

void help_exit()
{
    printf("PLACEHOLDER");
    clean_exit();
}

void clean_exit()
{
    GATEPASS("INTO CLEAN EXIT");
    if (options != NULL)
    {
        if (options->inputExists)
            free(options->args[0]);
        if (options->output)
            free(options->args[1]);
        if (options->directory)
            free(options->args[2]);
        if (options->threads)
            free(options->args[3]);
        for (int i = 0; i < 4; ++i) options->args[i] = NULL;
        free(options);
        options = NULL;
        GATEPASS("OPTIONS FREED SUCCESSFULLY");
    }
}


options_t* getoptions(int argc, char** argv)
{
    const char missing_input[] = "input file(i)";
    const char missing_direc[] = "directory of crackers (d)";
    char c = 0;
    GATEPASS("INTO GETOPTIONS");
    options = (options_t*) malloc(sizeof(options_t));
    *options = option_init;
    perror("BAD OPTARG");
    while ((c = getopt(argc,argv,":i:o:d:t:vhn")) != -1)
    {
        switch(c)
        {
            case 'i': //INPUT
                SAY("INPUT FOUND");
                options->inputExists = true;
                options->args[0] = strdup(optarg);
                break;
            case 'o': //OUTPUT
                SAY("OUTPUT FOUND");
                options->output = true;
                options->args[1] = strdup(optarg);
                break;
            case 'd': //DIRECTORIES
                SAY("DIRECTORY FOUND");
                options->directory = true;
                options->args[2] = strdup(optarg);
                break;
            case 't': //THREADS
                SAY("THREADS FOUND");
                options->threads = atoi(optarg);
                options->args[3] = optarg;
                break;
            case 'v': //VERBOSE
                SAY("VERBOSE ON");
                options->verbose = true;
                break;
            case 'h':
                SAY("HELP EXIT");
                options->help = true;
                help_exit();
                break;
            case 'n':
                SAY("NICE TOGGLED");
                options->nice = true;
                break;
            case '?':
                fprintf(stderr, "*** Unrecognized command line option ***\n*** Showing help and exiting...\n");
                help_exit();
                break;
            case ':':
                fprintf(stderr, "*** Missing command line option for: %c, exiting...\n", c);
                clean_exit();
                break;
            default:
                fprintf(stderr, "UNKNOWN ERROR OCCURRED, exiting ");
                SHOWI("OPT WAS: ", c);
                GATEPASS("UNKNOWN ERROR IN GETOPTIONS");
                clean_exit();
                break;
        }
    }
    if (c == 0) 
    {
        SAY("NO ARGUMENTS FOUND? EXITING OPTARGUMENT");
        fprintf(stderr, "*** No arguments provided, showing help and exiting...\n");
        help_exit();
    }
    if ((!options->inputExists || !options->directory))
    {
        SAY("MISSING REQUIRED OPTION");
        fprintf(stderr, "Missing command line options");
        if (!options->inputExists && !options->directory)
            fprintf(", both %s AND %s\n", missing_input, missing_direc);
        else if (!options->inputExists)
            fprintf(", %s, Need passwords in order to hash!\n", missing_input);
        else
            fprintf(", %s, Need dictionary to brute force passwords, else locked!\n", missing_direc);
        fprintf(stderr, "*** Showing help and exiting...\n");
        help_exit();
    }
    return options;
}

int main(int argc, char** argv)
{
    options = getoptions(argc,argv);
    
