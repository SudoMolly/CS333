#pragma once
#ifndef header
//**------------------------------|DOCUMENTATION|----------------------**/
/*
    * Molly Diaz, modiaz@pdx.edu, CS333 T-R Fall 25
    * Lab 3: Threaded cryptography
    *
    *
*/
//**------------------------------| INCLUDES   |-----------------------**/
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


//**------------------------------| DEFINES    |-----------------------**/
//----DEBUG----//
# define DEBUG
#ifdef DEBUG
    #define SAY(string) fprintf(stderr, "%s\n", string)
    #define SHOW(str1, str2) fprintf(stderr, "%s%s\n", str1,str2)
    #define SHOWI(str1,int1) fprintf(stderr, "%s%d\n",str1,int1)
    #define GATEPASS(ident) fprintf(stderr,"PASSED GATE %s ON LINE %d\nFunction: %s\tFile: %s\n------\n", ident, __LINE__, __func__, __FILE__)
    #define LOOPI(iden,i) fprintf(stderr, "In loop %s%d\n", iden,i)
#endif
#ifndef DEBUG
    #define SHOW(var)
    #define SHOWI(str1, int1)
    #define GATEPASS(ident)
    #define LOOPI(iden,i)
#endif


//----REQUIRED----//
#define NICE_VALUE 10
#define THREADS_DEF 1
#define OPTIONS ""
//**------------------------------| GLOBALS    |-----------------------**/
static int IN;
static int OUT;

//**------------------------------| STRUCTS    |-----------------------**/

typedef struct options_s{
    bool inputExists;
    bool output;
    bool directory;
    int threads;
    bool verbose;
    bool help;
    bool nice;
    char* args[4]; //input, output, directory, threads
} options_t;



static options_t* options = NULL;

//**------------------------------| PROTOTYPES |-----------------------**/

//nice
void help_exit();

//Destroys and cleans up any global variables, 
// **NOT THREAD SAFE!**
void clean_exit();

//Function to get options, for compactness
options_t* getoptions(int argc, char** argv);

#endif