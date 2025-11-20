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

[[maybe_unused]] static const char *algorithm_string[] = {
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
#include <stddef.h>
#include <sys/time.h>
#include <pthread.h>
#include <crypt.h>


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
#define MCS_TO_MS 10000
#define CRACKED_ALLOC 12
#define CRACKED_START "cracked  "
#define FAILED_ALLOC 21
#define FAILED_START "*** failed to crack  "
//#define 
//**------------------------------| GLOBALS    |-----------------------**/
static int HASH_IN [[maybe_unused]] = STDIN_FILENO;
static int PLAIN_IN[[maybe_unused]] = STDIN_FILENO;
static int OUT [[maybe_unused]] = STDOUT_FILENO;
static int VOUT [[maybe_unused]] = STDERR_FILENO;
static bool error_occurred = true;
static bool HASH_OPEN = false;
static bool PLAIN_OPEN = false;
static char* FILE_BUFFER = NULL;
static char** GLOBAL_HASH_LIST = NULL;
static char** GLOBAL_PLAIN_LIST = NULL;
static char** GLOBAL_CRACKED_LIST = NULL;
static int GLOBAL_HASH_NUM = 0;
static int GLOBAL_PLAIN_NUM = 0;
static struct timeval * times = NULL;
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

typedef struct thread_hash_data_s{
    char* line;
    int encounters[ALGORITHM_MAX];
    double time;
} thread_data;


static options_t* options = NULL;
static thread_data* data = NULL;

//**------------------------------| PROTOTYPES |-----------------------**/

thread_data* set_thread_data(thread_data* data);

void be_nice(void);
char* strnstr(char* hay, char needle, int max_count, bool flush);
char** count_n_assign(char** buff, int * toAdj);

//nice
void help_exit(void);

//Destroys and cleans up any global variables, 
// **NOT THREAD SAFE!**
void clean_exit(void);
void cleanAllRaggedArrays(void);
void open_file(char* plain_file, char* hash_file);
long init_buf(long buf_size);
int read_file(int fd);
int determine_type(char first, char second);
double elapse_time(struct timeval* t0, struct timeval* t1);
int get_next_thread(void);
bool crack_attempt(char* plain, char* hash);
void cracked_str(char* key, char* hash, int i);
void* decode(void* vid);

//Function to get options, for compactness
options_t* getoptions(int argc, char** argv);

#endif
