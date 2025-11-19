#ifndef header
//**------------------------------|DOCUMENTATION|----------------------**/
/*
    * Molly Diaz, modiaz@pdx.edu, CS333 T-R Fall 25
    * Lab 3: Threaded cryptography
    *
    *
*/

//**------------------------------| HIS                   |-----------------------**/

#ifndef _THREAD_HASH_H
// R Jesse Chaney
// rchaney@pdx.edu
# define _THREAD_HASH_H

# define OPTIONS "i:o:d:hvt:n"

# define MICROSECONDS_PER_SECOND 1000000.0

# ifndef NICE_VALUE
#  define NICE_VALUE 10
# endif // NICE_VALUE

# define FOREACH_ALGORITHM(ALGORITHM) \
        ALGORITHM(DES)           \
        ALGORITHM(NT)            \
        ALGORITHM(MD5)           \
        ALGORITHM(SHA256)        \
        ALGORITHM(SHA512)        \
        ALGORITHM(YESCRYPT)      \
        ALGORITHM(GOST_YESCRYPT) \
        ALGORITHM(BCRYPT)        \
        ALGORITHM(ALGORITHM_MAX)

# define GENERATE_ENUM(ENUM) ENUM,
# define GENERATE_STRING(STRING) #STRING,

typedef enum hash_algorithm_e {
    FOREACH_ALGORITHM(GENERATE_ENUM)
} hash_algorithm_t;

static const char *algorithm_string[] = {
    FOREACH_ALGORITHM(GENERATE_STRING)
};
#define BUF_SIZE 1000

#endif // _THREAD_HASH_

//**------------------------------| INCLUDES   |-----------------------**/
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


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
#define THREADS_DEF 1
//#define 
//**------------------------------| GLOBALS    |-----------------------**/
static int KIN [[maybe_unused]] = STDIN_FILENO;
static int DIN[[maybe_unused]] = STDIN_FILENO;
static int OUT [[maybe_unused]] = STDOUT_FILENO;
static bool error_occurred = true;
static bool KIN_OPEN = false;
static bool DIN_OPEN = false;
static char FILE_BUFFER[BUF_SIZE];
static char** GLOBAL_HASH_LIST = NULL;
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

typedef int HASH_COLLECTION[9];

static options_t* options = NULL;

//**------------------------------| PROTOTYPES |-----------------------**/
void be_nice(void);

//nice
void help_exit(void);

//Destroys and cleans up any global variables, 
// **NOT THREAD SAFE!**
void clean_exit(void);

//Function to get options, for compactness
options_t* getoptions(int argc, char** argv);

#endif
