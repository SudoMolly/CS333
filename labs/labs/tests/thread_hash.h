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
    #define SAY(string) //fprintf(stderr, "%s\n", string)
    #define SHOW(str1, str2) fprintf(stderr, "%s%s", str1,str2)
    #define SHOWI(str1,int1) //fprintf(stderr, "%s%d\n",str1,int1)
    #define GATEPASS(ident) //fprintf(stderr,"PASSED GATE %s ON LINE %d\nFunction: %s\tFile: %s\n------\n", ident, __LINE__, __func__, __FILE__)
    #define LOOPI(iden,i) //fprintf(stderr, "In loop %s%d\n", iden,i)
#endif
#ifndef DEBUG
    #define SAY(string)
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
#define PROMPT_FAILED_START "*** failed to crack  "

#define VER_TEMPLATE \
"\
thread:  #  XXXX.XX sec              DES:  XXXX               NT:  XXXX              MD5:  XXXX           SHA256:  XXXX           SHA512:  XXXX         YESCRYPT:  XXXX    GOST_YESCRYPT:  XXXX           BCRYPT:  XXXX  total:  XXXXXXX  failed:  XXXXXXX\
"
#define VERBOSE_ALG_SPACING 20
#define VERBOSE_ALG_LENGTH 4
#define THREAD_PREFIX 11
#define SECOND_START THREAD_PREFIX + 1 //12
#define SECOND_DOT SECOND_START + 4 //16
#define SECOND_END SECOND_DOT + 2 //18
#define SECOND_LENGTH 7

#define SEC_DES_SPACING VERBOSE_ALG_SPACING + 4 //20
#define DES_START SECOND_END + SEC_DES_SPACING + 1 //43
#define DES_END DES_START + VERBOSE_ALG_LENGTH - 1 //46

#define DES_NT_SPACING VERBOSE_ALG_SPACING
#define NT_START DES_END + DES_NT_SPACING + 1 //67
#define NT_END NT_START + VERBOSE_ALG_LENGTH - 1 //70

#define NT_MD5_SPACING VERBOSE_ALG_SPACING
#define MD5_START NT_END + NT_MD5_SPACING + 1 //91
#define MD5_END MD5_START + VERBOSE_ALG_LENGTH - 1 //94

#define MD5_SHA256_SPACING VERBOSE_ALG_SPACING
#define SHA256_START MD5_END + MD5_SHA256_SPACING + 1 //115
#define SHA256_END SHA256_START + VERBOSE_ALG_LENGTH - 1 //118

#define SHA256_SHA512_SPACING VERBOSE_ALG_SPACING
#define SHA512_START SHA256_END + SHA256_SHA512_SPACING + 1 //139
#define SHA512_END SHA512_START + VERBOSE_ALG_LENGTH - 1 //142

#define SHA512_YES_SPACING VERBOSE_ALG_SPACING
#define YES_START SHA512_END + SHA512_YES_SPACING + 1 //163
#define YES_END YES_START + VERBOSE_ALG_LENGTH - 1 //166

#define YES_GY_SPACING VERBOSE_ALG_SPACING
#define GY_START YES_END + YES_GY_SPACING + 1 //187
#define GY_END GY_START + VERBOSE_ALG_LENGTH - 1 //190

#define GY_B_SPACING VERBOSE_ALG_SPACING
#define B_START GY_END + GY_B_SPACING + 1 //211
#define B_END B_START + VERBOSE_ALG_LENGTH - 1 //214

#define B_TOTAL_SPACING 10
#define TOTAL_START B_END + B_TOTAL_SPACING + 1 //225
#define TOTAL_LENGTH 7
#define TOTAL_END TOTAL_START + TOTAL_LENGTH - 1 //231

#define TOTAL_FAILED_SPACING 11 //??? 
#define FAILED_START TOTAL_END + TOTAL_FAILED_SPACING + 1 //243
#define FAILED_LENGTH 7
#define FAILED_END FAILED_START + FAILED_LENGTH - 1 //249

#define VERBOSE_END FAILED_END + 1
#define VERBOSE_LENGTH VERBOSE_END + 1

#define VERBOSE_ARRAYS_OFFSET {SECOND_START, DES_START, NT_START, MD5_START, SHA256_START, SHA512_START, YES_START, GY_START, B_START, TOTAL_START, FAILED_START} 
#define VERBOSE_ARRAYS_LENGTH {SECOND_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, VERBOSE_ALG_LENGTH, TOTAL_LENGTH, FAILED_LENGTH}

/*

*/
#define ZERO_OUT(chars, len) {for(int i = 0; i < len; ++i) chars[i] = ' ';}
//#define 
//**------------------------------| GLOBALS    |-----------------------**/
static int HASH_IN [[maybe_unused]] = STDIN_FILENO;
static int PLAIN_IN[[maybe_unused]] = STDIN_FILENO;
static int OUT [[maybe_unused]] = STDOUT_FILENO;
static int VOUT [[maybe_unused]] = STDERR_FILENO;
static bool error_occurred = true;
static bool HASH_OPEN = false;
static bool PLAIN_OPEN = false;
static bool OUT_OPEN = false;
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
    int failures;
    int total;
    char verbose[VERBOSE_LENGTH];
    int thread_num;
} thread_data;


static options_t* options = NULL;
static thread_data* data = NULL;

//**------------------------------| PROTOTYPES |-----------------------**/

void set_thread_data(void);

void be_nice(void);
char* strnstr(char* hay, char needle, int max_count, bool flush);
char** count_n_assign(char** buff, int * toAdj);

//nice
void help_exit(void);

//Destroys and cleans up any global variables, 
// **NOT THREAD SAFE!**
void clean_exit(void);
void cleanAllRaggedArrays(void);
void open_file(char* plain_file, char* hash_file, char* out_file);
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
