//**-----------------------------------------INCLUDES ------------------------------------*/
//#include <openssl/md4.h>
//#include <md4.h>
#include <openssl/md4.h>
#include "arvik.h"

#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>


typedef struct my_arvik_header_s {
	char arvik_name[ARVIK_NAME_LEN];          // Member file name, sometimes < terminated.
	char arvik_date[ARVIK_DATE_LEN];          // File date, decimal seconds since Epoch.
	char arvik_uid[ARVIK_UID_LEN];            // User ID, in ASCII decimal.
	char arvik_gid[ARVIK_GID_LEN];            // Group ID, in ASCII decimal.
	char arvik_mode[ARVIK_MODE_LEN];          // File mode, in ASCII octal.
	char arvik_size[ARVIK_SIZE_LEN];          // File size, in ASCII decimal.
	char arvik_term[ARVIK_TERM_LEN + 1];          // Always contains ARVIK_TERM.
} my_arvik_header_t;

typedef struct my_arvik_footer_s {
	char md4sum_header[MD4_DIGEST_LENGTH * 2];
	char md4sum_data[MD4_DIGEST_LENGTH * 2];
	char arvik_term[ARVIK_TERM_LEN + 1];          // Always contains ARVIK_TERM.
} my_arvik_footer_t;

//#include <openssl/evp.h>
#include <openssl/err.h>
//**-----------------------------------------DEFINES ------------------------------------*/

//** DEBUG DEFINES BEGIN    */###########################################
//--**DISABLE**--//
# define DEBUG 
//--**ENABLE**--//

#ifndef DEBUG
# define SHOW(name, var)
# define GATEPASS()
# define SHOWI(name, var)
# define NEWLINE()
#endif
#ifdef DEBUG
#   define SHOW(name, var) fprintf(stderr, "%s %s", name, var)
    // 3 DIGIT CODE: 
    /*
    X## = In line count, starting from 1
    X99 = end

    0XX = MAIN      
    1XX = CREATE    
    2XX = EXTRACT
    3XX = TOC
    4XX = TOC_VERB
    5XX = TOC_VALID
    6XX = TOC_VERB_VALID
    7XX = FILENAME
    8XX = HELP
    9XX = OTHER
    */
#   define GATEPASS(num) fprintf(stderr,"-->\nPASSED GATE %3d ON LINE: %d IN FUNCTION: %s<--\n",num, __LINE__, __func__)
#   define SHOWI(name, var) fprintf(stderr, "%s %d", name, var)
#   define NEWLINE() fprintf(stderr, "\n");
#endif 

# define CREATE_ERROR(why) fprintf(stderr,"Error when trying to create file because: %s", why);
# define EXTRACT_ERROR(why) fprintf(stderr,"Error when trying to extract file because: %s", why);
# define TOC_ERROR(why) fprintf(stderr,"Error when trying to make TOC because: %s", why);

//** DEBUG DEFINES END      */

//** TOC DEFINES BEGIN      */############################################
//----SIMPLE
# define TOC_SIMPLE_MAX_LEN 31 //NO SPACES AFTER
//----VERBOSE
# define TOC_TERM '\n'
# define TOC_PREFIX 4           //(SPACES)---------------------------
# define TOC_TITLE_TOTAL_LEN 42


//mode
# define TOC_MODE_TYPE_LEN 5
# define TOC_MODE_SPACING 7
# define TOC_MODE_DATA_LEN 9
# define TOC_MODE_TOTAL_LEN 26

//UID, GID, SIZE                    //SPACES--TYPE--SPACES--INT--STRING--
# define TOC_UGS_SPACING 2          //----------------------INT(--)STRING
# define TOC_UGS_END_INT 25         //(SPACES--TYPE--SPACES--INT)--------
//UID
# define TOC_UID_TYPE_LEN  4        //-------(TYPE)---------------------
# define TOC_UID_LEN_INT 17         //--------------(SPACES--INT)--------
//GID
# define TOC_GID_TYPE_LEN  4        //-------(TYPE)---------------------
# define TOC_GID_LEN_INT 17         //--------------(SPACES--INT)--------
//SIZE
# define TOC_SIZE_TYPE_LEN  5       //-------(TYPE)----------------------
# define TOC_SIZE_LEN_INT 16        //--------------(SPACES--INT)--------
# define TOC_SIZE_TOTAL_LEN 33
# define TOC_COL_1_TOTAL_LEN 27


//mtime
# define TOC_MTIME_TYPE_LEN 6
# define TOC_MTIME_SPACING 7
# define TOC_MTIME_MONTH_LEN 3
# define TOC_MTIME_MONTH_DAY_SPACING 2
# define TOC_MTIME_DAY_LEN 1
# define TOC_MTIME_DAY_HOUR_SPACING 1
# define TOC_MTIME_HOUR_LEN 2
# define TOC_MTIME_HOUR_MIN_SPACING 1 //colon
# define TOC_MTIME_MIN_LEN 2
# define TOC_MTIME_MIN_YEAR_SPACING 1
# define TOC_MTIME_TOTAL_LEN 34

# define TOC_MTIME_MONTH_INDEX 16
//21
# define TOC_MTIME_DAY_INDEX TOC_MTIME_MONTH_INDEX + TOC_MTIME_MONTH_LEN + TOC_MTIME_MONTH_DAY_SPACING
//23
# define TOC_MTIME_HOUR_INDEX TOC_MTIME_DAY_INDEX + TOC_MTIME_DAY_LEN + TOC_MTIME_DAY_HOUR_SPACING
//25
# define TOC_MTIME_COLON_INDEX TOC_MTIME_HOUR_INDEX + TOC_MTIME_HOUR_LEN
//26
# define TOC_MTIME_MIN_INDEX TOC_MTIME_COLON_INDEX + TOC_MTIME_HOUR_MIN_SPACING
//29
# define TOC_MTIME_YEAR_INDEX TOC_MTIME_MIN_INDEX + TOC_MTIME_MIN_LEN +  TOC_MTIME_MIN_YEAR_SPACING

# define TOC_MTIME_RESULT_LEN 18
//MD4
# define TOC_MD4_TOTAL_LEN 49
# define TOC_MD4_CHECKSUM_LEN 32
//header md4
# define TOC_HEADER_MD4_TYPE_LEN 11
# define TOC_HEADER_MD4_TYPE_CHECKSUM_SPACING 1
//data md4
# define TOC_DATA_MD4_TYPE_LEN 9
# define TOC_DATA_MD4_TYPE_CHECKSUM_SPACING 3

//---------

//-----verify
# define TOC_VERIFY_TITLE_OVER_LEN 30
# define TOC_VERIFY_TITLE_OVER_TOTAL_LEN 33
//I DONT KNOW WHY
# define TOC_VERIFY_TITLE_UNDER_LEN 27
# define TOC_VERIFY_TITLE_UNDER_TOTAL_LEN 28

//** TOC DEFINES END        */

//** OTHER DEFINES BEGIN    */############################################
# define SAY(str) fprintf(stderr, "%s\n", str)
# define INITARRAY(buff, len) for (int y = 0; y < len; ++y) buff[y] = ' '
# define INITCHAR(buff, len) for (int z = 0; z < len; ++z) buff[z] = '\0'
# define INITZERO(buff, len) for (int x = 0; x < len; ++x) buff[x] = '0'
# define EOFCHECK(bytes) if (bytes == 0) break
# define SPACEPRINT(spaces) for (unsigned int i = 0; i < spaces; ++i) printf(" ");
# define ARVIK_TERM_BEGIN '$'
# define ARVIK_TERM_END '\n'
# pragma GCC diagnostic ignored  "-Wdeprecated"
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//** OTHER DEFINES END      */

//** CREATE DEFINES BEGIN    */############################################
# define ARVIK_HEADER_LEN 78
# define ARVIK_FOOTER_LEN 66
# define ARVIK_TAG_LEN 10
# define RDWR_BUFF_SIZE 10
# define MD4_HASH_MAX_LEN (MD4_DIGEST_LENGTH * 2) //alias, since inconsistent naming scheme keeps messing me up
#ifndef MD4_DIGEST_STRING_LENGTH
    # define MD4_DIGEST_STRING_LENGTH 33
#endif
//** CREATE DEFINES END      */

//** FAILURE DEFINES BEGIN   */############################################
# define WRITE_FAIL 11
# define OPEN_FAIL 12
# define UNKNOWN_FAIL 13
# define BAD_DATE 14
# define BAD_UID 15
# define BAD_GID 16
# define BAD_MODE 17
# define BAD_SIZE 18
# define BAD_TERM 19
# define MTIME_FAIL 20


//NEXT LEVEL
# define TOC_ERROR_BEGIN 1000
# define TOC_ERROR_TIME 1009



# define MD4_FAIL MD4_ERROR
//** FAILURE DEFINES END     */

//**-----------------------------------------GLOBALS ------------------------------------*/
//global triggers (enum didnt make sense)
static bool extract = false;
static bool create = false;
static bool toc = false;
static bool filename = false;
static bool help = false;
static bool verbose = false;
static bool verify = false;
static char* out_string = NULL;

//global vars, less data transfer
static int argCount = 0;
static int fileC = 0;
static int currentFD [[maybe_unused]]= 0;
static int IN = STDIN_FILENO;
static int OUT = STDOUT_FILENO;
static int writeError = 0;
static int readError = 0;
static int intToStrSize_Len = 0;
static bool EOF_REACHED = false;
static int IN_TOC = STDIN_FILENO;
static bool HEAD_MD4_FAILURE = false;
static bool FOOT_MD4_FAILURE = false;
static bool MD4_FAILURE = false;
//static ssize_t MD4_bytesRead = 0;

//const char begin[] = "#<arvik4>\n";


//toc-verbose
const char VERBOSE_FILE_NAME[] = "file name: ";
const char VERBOSE_MODE[] = "    mode:";
const char VERBOSE_UID[] = "    uid:";
const char VERBOSE_GID[] = "    gid:";
const char VERBOSE_SIZE[] = "    size:";
const char VERBOSE_MTIME[] = "    mtime:";//      MON  D HH:MM YEAR\n";

const char VERBOSE_MTIME_FORMAT[] = "%b %e %R %Y";
const char VERBOSE_HEADER_MD4[] = "    header md4: ";
const char VERBOSE_DATA_MD4[] = "    data md4:   ";

//toc-verify
const char VERIFY_HEADER_FAIL[] = "         header fails validation     \n";
const char VERIFY_HEADER_SUCCESS[] = "    header passes validation\n";
const char VERIFY_DATA_FAIL[] =   "         data fails validation     \n";
const char VERIFY_DATA_SUCCESS[] =   "    data passes validation\n";

//Always on verbose, only in verify if wrong
const char VERBOSE_VERIFY_HEADER_MD4_CONTENT[] = "    header md4 content: ";
const char VERBOSE_VERIFY_HEADER_MD4_FOOTER[] =  "    header md4 footer : ";
const char VERBOSE_VERIFY_DATA_MD4_CONTENT[] = "    data md4 content:   ";
const char VERBOSE_VERIFY_DATA_MD4_FOOTER[] =  "    data md4 footer :   ";

//Only ever verify (im going to find you chaney...)
const char VERBOSE_VERIFY_FAILURE[] = "     validation failure for 1 or more archive members     \n";

char VERBOSE_FILE_LINE_3[TOC_COL_1_TOTAL_LEN];
char VERBOSE_FILE_LINE_4[TOC_COL_1_TOTAL_LEN];
char VERBOSE_FILE_LINE_5[TOC_COL_1_TOTAL_LEN];
char VERBOSE_FILE_LINE_6[TOC_MTIME_TOTAL_LEN];
char VERBOSE_FILE_LINE_7[TOC_MD4_TOTAL_LEN];
char VERBOSE_FILE_LINE_8[TOC_MD4_TOTAL_LEN];
char VERBOSE_TIME_RESULT[TOC_MTIME_RESULT_LEN];
static struct tm* VERBOSE_TIME_STRUCT = NULL;


static struct stat holdTheHolder [[maybe_unused]];



//

//**-----------------------------------------STRUCTS ------------------------------------*/


typedef struct toc_verbose{
    char file_nameLINE[TOC_TITLE_TOTAL_LEN];
    char mode[TOC_MODE_TOTAL_LEN];
    char* uid; //No known ending
    char* gid; //same here
    char size[TOC_SIZE_TOTAL_LEN]; //size is known
    char mtime[TOC_MTIME_TOTAL_LEN];
    char header_md4[TOC_MD4_TOTAL_LEN];
    char data_md4[TOC_MD4_TOTAL_LEN];
}toc_verbose_t;

typedef struct toc_verify_s{
    bool IS_VERBOSE;

    bool titleISUnder; //SWITCH BETWEEN TWO TITLE STANDARDS
    char titleUnderStr[TOC_VERIFY_TITLE_UNDER_TOTAL_LEN];
    char titleOverStr[TOC_VERIFY_TITLE_OVER_TOTAL_LEN];

    struct header_s{
        bool headerPass;
        const char* pass;
        char* content;
        char* footer;
    } header_t;

    struct data_s{
        bool dataPass;
        const char* pass;
        char* content;
        char* footer;
    } data_t;

    bool anyfail;
} toc_verify_t;

typedef struct arvik_lines_s{
    char start[ARVIK_TAG_LEN];
    char header[ARVIK_HEADER_LEN];
    char footer[ARVIK_FOOTER_LEN];
} arvik_lines_t;

//**-----------------------------------------ALL-POINTERS--------------------------------------*/

static void* holder = NULL;
//static int* fileV_INT = NULL;
static char* currentARG = NULL;
static char* intStrBUFF = NULL;
static char* inBUFF = NULL;
static char* outBUFF = NULL;
//static char** fileV_STR = NULL;
static struct stat* statHold = NULL;
static my_arvik_header_t* header = NULL;
static my_arvik_footer_t* footer = NULL;
static toc_verbose_t* verboseDATA = NULL;
static toc_verify_t* verifyDATA = NULL;
static arvik_lines_t* arvikLINES = NULL;

//static MD4_CTX* MD4_context = NULL;
static uint8_t MD4_hash[MD4_DIGEST_STRING_LENGTH];
//static unsigned int* MD4_hash_len = NULL;
static MD4_CTX* ctx = NULL;
//EVP_MD_CTX* EVP_context = NULL;

static char* EXTRACT_header = NULL;
static char* EXTRACT_footer = NULL;
static char* currentFileName = NULL;
static int EXTRACT_MAX_READ = 0;
static int EXTRACT_current_read = 0;
static int EXTRACT_total_read = 0;
static char EXTRACT_GUESS_HEADER_HASH[MD4_HASH_MAX_LEN];

static DIR* dir [[maybe_unused]] = NULL;
//static char* EVP_message = NULL;
//static size_t EVP_message_len = 0;
//static unsigned char** EVP_digest = NULL;
//static unsigned int* EVP_digest_len = NULL;

//**-----------------------------------------PROTOTYPES ------------------------------------*/
bool allZeros(const unsigned char arr[], int len);
bool notAllNumOrSpace(const char* arr, int len);
unsigned long intToStrSize(void);
void intToStr(void);
void md4_init(void);
void md4_destruct(void);
void read_write(void);
void getStat(char* path);
void getFileName(void);
void decipherHead(char line1[]);
void decipherFoot(char line[]);
void change_meta_data(void);
void startFoot(void);
void endFoot(void);
void TOC_SIMPLE(void);
void TOC_HASH(bool vT, bool veT);
void TOC_VERIFY(void);
void TOC_VERBOSE(void);
void TOC_VERIFY_VERBOSE(void);
toc_verify_t* TOC_VERIFY_STRUCT_INIT(toc_verify_t* outP, bool verbosed);
//
void* easy_free(void* this);

void setHead(char* path);

void _read(char* path);

//-c
void _create(void);
//-x
void _extract(void);

//-v && -V && -f do not need functions


//-t
void tocSimple(int count, char**names, int fd);
//-t && -v
void tocVerbose(toc_verbose_t* toc, int fd);
//-t && -V
void tocVerify(toc_verify_t* toc, int fd);
//-t && -V && -v
void tocVerifyVerbose(toc_verify_t* toc, int fd);

//-h
void displayHelp(void);

void EXIT_DESTROY(int error, const char* msg);

char* myStrncpy(int bytes, ...);
void writeLoop(char* buf, int max);


//**------NEW PROTOS-----*/
//for loop going through file, first opening, stating & creating header then writing header
//then reading & writing file data, then once all data collected, write out finished footer
void create_arvik(char** ARGUMENT_LIST, int ARG_COUNT, int INDEX_OF_FILES_START);

//A while loop that will read through the data in a file, and hash it
//child file and should not be called directly
void __create_arvik_data(void);

//converts global MD4_hash to hex and stores in
//global HASHHOLD
void hash_convert(void);

//Just changes the time modified && time accessed to appropiate nums
void __extract_arvik_detail_modify(void);

//Actually creates the file arvik mentions
void __extract_arvik_create(int num);

//*----EXTRACT----*//

//compares the expected hash and the calculated hash
//iow, will return iff no data loss has occurred
bool __extract_arvik_hash_cmp(void);

//Reads the data files and hashes
void __extract_arvik_hash_lines(void);

//Extracts the header data to be read by subsequent functions
void __extract_arvik_header_data(void);

//Extracts the arvik to file directory
void extract_arvik(void);
//**-----------------------------------------FUNCTIONS ------------------------------------*/

struct create_s{
    char header[ARVIK_HEADER_LEN];
    char footer[ARVIK_FOOTER_LEN];
} create_d = {
    "                                                                            $\n",
    "                                                                $\n"
    //                                     DDDDDDDDD     UUUUU GGGGG MMMMMM  SS          $\n
};

typedef struct extract_data_s{
    char fileName[ARVIK_NAME_LEN + 1];
    unsigned long date;
    int uid;
    int gid;
    int mode;
    long size;
    char hash_head[MD4_DIGEST_STRING_LENGTH - 1];
    char hash_foot[MD4_DIGEST_STRING_LENGTH];
} extract_data_t;

#define DATE_START 30
#define DATE_END 43
#define UID_START 44
#define UID_END 48
#define GID_START 50
#define GID_END 54
#define MODE_START 56
#define MODE_END 61
#define SIZE_START 64
#define SIZE_END 75

static int CREATE_SIZE = sizeof(create_d);
static int EXTRACT_SIZE = sizeof(extract_data_t);

typedef struct create_s create_t;


static create_t * create_t_list = NULL;
static create_t * extract_t_list = NULL;
static extract_data_t* extract_t_data = NULL;
int* read_file_fail_i = NULL;
static char INTHOLD[21];
static char HASHHOLD[33];
static char DATANUMHOLD[15];

//** ----CREATE---- */

void hash_convert(void)
{
        int org_hash_i;
        unsigned int hold;
        org_hash_i = 0;
        INITZERO(HASHHOLD, MD4_DIGEST_STRING_LENGTH - 1);
        for (int i = 0; i < MD4_DIGEST_STRING_LENGTH - 1; i += 2)
        {
            hold = MD4_hash[org_hash_i];
            sprintf(HASHHOLD + i, "%02x", hold);
            ++org_hash_i;
        }
}

void create_arvik(char** argv, int argc , int optindex)
{
    int fileIndex;
    char* file;
    create_t* alias;
    int name, date, uid, gid, mode, size = 0;
    read_file_fail_i = (int*) calloc(fileC,sizeof(int));
    create_t_list = (create_t*) calloc(fileC, CREATE_SIZE);

    fileIndex = 0;
    for (int i =  optindex; i < argc; ++i)
    {
        file = argv[i];
        if ((IN = open(file, O_RDONLY)) == -1)EXIT_DESTROY(READ_FAIL, "FAILED TO READ FILE");

        create_t_list[i] = create_d;
        alias = create_t_list + i;
        name = strlen(file);
        if (name > ARVIK_NAME_LEN) memcpy(create_t_list[i].header, file, ARVIK_NAME_LEN);
        else{                      memcpy(create_t_list[i].header, file, name); 
                                   alias->header[name] = ARVIK_NAME_TERM;
        }

        getStat(file);

        INITCHAR(INTHOLD,21);
        sprintf(INTHOLD, "%lu", statHold->st_mtim.tv_sec);
        date = strlen(INTHOLD);
        if (date <= ARVIK_DATE_LEN)  memcpy(alias->header + DATE_START, INTHOLD, date);
        else                        memcpy(alias->header + DATE_START, INTHOLD, ARVIK_DATE_LEN);

        INITCHAR(INTHOLD, 21);
        sprintf(INTHOLD, "%d", statHold->st_uid);
        uid = strlen(INTHOLD);
        if (uid <= ARVIK_UID_LEN)    memcpy(alias->header + UID_START, INTHOLD, uid);
        else                         memcpy(alias->header + UID_START, INTHOLD, ARVIK_UID_LEN);

        INITCHAR(INTHOLD, 21);
        sprintf(INTHOLD, "%d", statHold->st_gid);
        gid = strlen(INTHOLD);
        if (gid <= ARVIK_GID_LEN)    memcpy(alias->header + GID_START, INTHOLD, gid);
        else                         memcpy(alias->header + GID_START, INTHOLD, ARVIK_GID_LEN);

        INITCHAR(INTHOLD, 21);
        sprintf(INTHOLD, "%o", statHold->st_mode);
        mode = strlen(INTHOLD);
        if (mode <= ARVIK_MODE_LEN)    memcpy(alias->header + MODE_START, INTHOLD, mode);
        else                         memcpy(alias->header + MODE_START, INTHOLD, ARVIK_MODE_LEN);

        INITCHAR(INTHOLD,21);
        sprintf(INTHOLD, "%lu", statHold->st_size);
        size = strlen(INTHOLD);
        if (size <= ARVIK_SIZE_LEN)  memcpy(alias->header + SIZE_START, INTHOLD, size);
        else                        memcpy(alias->header + SIZE_START, INTHOLD, ARVIK_SIZE_LEN);

        md4_init();
        MD4_Update(ctx, alias->header, ARVIK_HEADER_LEN);
        MD4_Final(MD4_hash, ctx);
        hash_convert();
        memcpy(alias->footer, HASHHOLD, MD4_DIGEST_STRING_LENGTH - 1);
        md4_destruct();
        write(OUT, alias->header, ARVIK_HEADER_LEN);
        md4_init();
        __create_arvik_data();
        hash_convert();
        memcpy(alias->footer + MD4_DIGEST_STRING_LENGTH - 1, HASHHOLD, MD4_DIGEST_STRING_LENGTH - 1);
        md4_destruct();
        write(OUT, alias->footer, ARVIK_FOOTER_LEN);

        if ((close(IN)) == -1) EXIT_DESTROY(READ_FAIL, "FAILED TO CLOSE FILE IN CREATE_MAIN");
        ++fileIndex;
    }
    create_t_list = easy_free(create_t_list);
}

void __create_arvik_data(void)
{
    bool eof_reached;
    bool data_actual;
    int read_count, write_count = 0;
    data_actual = eof_reached = false;
    if (outBUFF != NULL)
        outBUFF = easy_free(outBUFF);
    outBUFF = (char*) malloc(RDWR_BUFF_SIZE);
    while (!(EOF_REACHED = eof_reached))
    {
        read_count = read(IN,outBUFF,RDWR_BUFF_SIZE);
        if (read_count == -1) EXIT_DESTROY(READ_FAIL, "failed to read");
        else if (read_count == 0) 
        {
            SAY("END OF FILE REACHED");
            eof_reached = true;
        } else {
            write_count = write(OUT,outBUFF,read_count);
            if (write_count == -1) EXIT_DESTROY(WRITE_FAIL, "failed to write out to fd");

            if (MD4_Update(ctx, outBUFF, read_count) != 1) EXIT_DESTROY(MD4_ERROR, "Error when updating EVP digest");
        }
        if (!data_actual && read_count > 0) data_actual = true;
    }
    if (data_actual)
        write(OUT,"\n", 1);
    if ((MD4_Final(MD4_hash, ctx) != 1)) EXIT_DESTROY(MD4_ERROR, "Error when finalizing hash? EVP SUCKS! (not true)");
    outBUFF = easy_free(outBUFF);
}

//** ----EXTRACT---- */


void __extract_arvik_detail_modify(void)
{
    struct utimbuf buff;
    buff.actime = time(NULL);
    buff.modtime = extract_t_data->date;
    if (utime(extract_t_data->fileName, &buff) == -1) EXIT_DESTROY(EXTRACT_FAIL, "COULD NOT MODIFY TIMES ON FILE");
}

void __extract_arvik_create(int num)
{
    int prevOUT;
    int perms;
    int numWrote;
    
    prevOUT = OUT;
    perms = extract_t_data->mode / 1000;

    OUT = open(extract_t_data->fileName, O_CREAT | O_TRUNC, 0000 | perms/100 | perms/10 | perms % 10);
    if (OUT == -1) EXIT_DESTROY(CREATE_FAIL, "CANNOT CREATE FILE IN EXTRACT?");
    numWrote = write(OUT,outBUFF,num);
    if (numWrote == -1) EXIT_DESTROY(WRITE_FAIL, "CANNOT WRITE DE-ARVIK'D FILE");
    close(OUT);
    OUT = prevOUT;
}

bool __extract_arvik_hash_cmp(void)
{
    int num_read;
    int diff;
    bool result;
    result = true;
    if (outBUFF != NULL) outBUFF = easy_free(outBUFF);
    outBUFF = (char*) calloc(ARVIK_FOOTER_LEN, sizeof(char));
    num_read = read(IN, outBUFF,ARVIK_FOOTER_LEN);
    if (num_read == 0) EXIT_DESTROY(READ_FAIL, "NO FOOTER HASH!");
    if (num_read == -1) EXIT_DESTROY(READ_FAIL,"DONT KNOW WHAT TO TELL YOU HOSS, FOOTER HASH FAILED");

    if ((diff = strncmp(outBUFF ,extract_t_data->hash_head,(MD4_DIGEST_STRING_LENGTH * 2) - 3)) != 0) 
    {
        SHOWI("STRINGS NOT EQUAL DIFF: ",diff);
        SHOW("\noutBUFF: \n", outBUFF);
        NEWLINE();
        SHOW(extract_t_data->hash_head, "<");
        NEWLINE();
        NEWLINE();
        result = false;
        GATEPASS(-439);
    }
    GATEPASS(439);
    return result;
}

void __extract_arvik_hash_lines(void)
{
    int num_read;
    char* throwaway;
    int seek_num= 0;
    throwaway = (char*) malloc(1);
    seek_num = extract_t_data->size;

    md4_init();
    if (seek_num > 0)
    {
        if (outBUFF != NULL) outBUFF = easy_free(outBUFF);
        outBUFF = (char*) malloc(seek_num * sizeof(char));
        num_read = read(IN,outBUFF,seek_num);
        if (num_read == 0) EXIT_DESTROY(READ_FAIL,"COULDNT READ ARVIK FILE, NO DATA!");
        else if (num_read == -1) EXIT_DESTROY(READ_FAIL, "UNKNOWN READ ARVIK ERROR");
        MD4_Update(ctx,outBUFF , seek_num);
        read(IN, throwaway, 1);
        throwaway = easy_free(throwaway);
    }

    MD4_Final(MD4_hash, ctx);
    hash_convert();
    memcpy(extract_t_data->hash_foot, HASHHOLD, MD4_DIGEST_STRING_LENGTH);
    extract_t_data->hash_foot[MD4_DIGEST_STRING_LENGTH - 1] = '$';
    //create the file
    if (!__extract_arvik_hash_cmp()) EXIT_DESTROY(MD4_FAILURE, "HASHES WERE NOT EQUAL, DATA LOSS!");
    __extract_arvik_create(seek_num);
    md4_destruct();
    outBUFF = easy_free(outBUFF);
}

void __extract_arvik_header_data(void)
{
    int i;
    char current;
    char* currentO;
    extract_data_t* aliasO = NULL;
    create_t* aliasI = NULL;
    bool name_ended = false;
    if (extract_t_data != NULL)
        extract_t_data = easy_free(extract_t_data);
    extract_t_data = (extract_data_t*) malloc(EXTRACT_SIZE);

    aliasO = extract_t_data;
    aliasI = extract_t_list;

    GATEPASS(310);
    INITCHAR(aliasO->fileName, ARVIK_NAME_LEN + 1);
    while(!name_ended && i < ARVIK_NAME_LEN)
    {
        if (i == ARVIK_NAME_LEN) name_ended = true;
        else
        {
            current = aliasI->header[i];
            currentO = aliasO->fileName + i;
            if (current == ARVIK_NAME_TERM)
                name_ended = true;
            else *currentO = current;
        }
        ++i;
    }

    INITCHAR(DATANUMHOLD, 15);
    memcpy(DATANUMHOLD, aliasI->header + DATE_START, DATE_END - DATE_START + 1);
    aliasO->date = (unsigned long) atol(DATANUMHOLD);
    
    INITCHAR(DATANUMHOLD,15);
    memcpy(DATANUMHOLD, aliasI->header + UID_START, UID_END - UID_START + 1);
    aliasO->uid = atoi(DATANUMHOLD);
     
    INITCHAR(DATANUMHOLD,15);
    memcpy(DATANUMHOLD, aliasI->header + GID_START, GID_END - GID_START + 1);
    aliasO->gid = atoi(DATANUMHOLD);

    INITCHAR(DATANUMHOLD, 15);
    memcpy(DATANUMHOLD,aliasI->header + MODE_START, MODE_END-MODE_START + 1);
    aliasO->mode = atoi(DATANUMHOLD);

    INITCHAR(DATANUMHOLD, 15);
    memcpy(DATANUMHOLD,aliasI->header + SIZE_START, SIZE_END-SIZE_START + 1);
    aliasO->size = atol(DATANUMHOLD);

    if (ctx != NULL) 
        md4_destruct();
    md4_init();
    MD4_Update(ctx,aliasI->header,ARVIK_HEADER_LEN);
    MD4_Final(MD4_hash, ctx);
    hash_convert();
    memcpy(aliasO->hash_head, HASHHOLD, MD4_DIGEST_STRING_LENGTH);
    md4_destruct();
}

void extract_arvik(void)
{
    int num_read;
    bool eof_reached;
    create_t * alias_read;
    num_read = eof_reached = 0;
    if (outBUFF != NULL) outBUFF = easy_free(outBUFF);
    outBUFF = (char*) malloc(ARVIK_TAG_LEN + 1);
    INITARRAY(outBUFF, ARVIK_TAG_LEN);
    num_read = read(IN, outBUFF, ARVIK_TAG_LEN);
    if (strncmp(outBUFF,ARVIK_TAG, ARVIK_TAG_LEN)) EXIT_DESTROY(BAD_TAG, "*** failed to read tag");
    while (!(EOF_REACHED = eof_reached))
    {
        GATEPASS(301);
        if (extract_t_list == NULL) extract_t_list = easy_free(extract_t_list);
        extract_t_list = (create_t*) malloc(CREATE_SIZE);
        alias_read = extract_t_list;
        //read the header
        INITCHAR(alias_read->header,ARVIK_HEADER_LEN);
        num_read = read(IN, alias_read->header, ARVIK_HEADER_LEN);
        if (num_read == -1) EXIT_DESTROY(READ_FAIL,"Couldn't read file (-1 error)"); //UNKNOWN ERROR
        else if (num_read == 0) eof_reached = true; //END!
        else if (num_read != ARVIK_HEADER_LEN) EXIT_DESTROY(READ_FAIL, "COULDNT READ HEADER FILE"); //WRONG LENGTH
        else
        {
            //decipher_header
            GATEPASS(302);
            __extract_arvik_header_data();
            //hash data->compare->create file 
            GATEPASS(303);
            __extract_arvik_hash_lines();
            //add them times
            GATEPASS(304);
            __extract_arvik_detail_modify();
            GATEPASS(309);
        }
        extract_t_data = easy_free(extract_t_data);
        extract_t_list = easy_free(extract_t_list);
    }
}


//**------------------------------------------NEW UPDATED DATA-----------------------------*/

char* myStrncpy(int bytes, ...)
{
    int c;
    char a;
    char b;
    char* a_next;
    char* b_next;
    va_list vargs;
    if (holder != NULL) free(holder);

    c = bytes;
    bytes = 2;
    va_start(vargs, bytes);
    a = (char) va_arg(vargs, int);
    b = (char) va_arg(vargs, int);
    holder = a_next = &a;
    b_next = &b;
    for (int j = 0; j < c; ++j)
    {
        a_next = a_next + j;
        b_next = b_next + j;
        *a_next = b;
    }
    va_end(vargs);
    return holder;
}

bool allZeros(const unsigned char arr[], int len)
{
    for (int i = 0; i < len; ++i)
        if (arr[i] != ' ') return false;
    return true;
}

bool notAllNumOrSpace(const char* arr, int len)
{
    for (int i = 0; i < len; ++i)
        if (arr[i] != ' ' && (isalpha(arr[i]))) return false;
    return true;
}

void writeLoop(char* buf, int max)
{
    for (int i = 0; i < max; ++i)
    {
        if (buf[i] != ' ') continue;
        write(OUT, buf + i, 1);
    }
}

unsigned long intToStrSize(void)
{
    unsigned long size;
    unsigned long hold;
    size = 1;
    hold = *((unsigned long*) holder);
    do
    {
        hold /= 10;
        ++size;
    } while(hold != 0);
    return size;
}

void intToStr(void)
{
    unsigned long numConvert;
    unsigned long size;
    size = intToStrSize();
    intToStrSize_Len = size;
    numConvert = *((unsigned long*) holder);
    if (intStrBUFF != NULL)
        free(intStrBUFF);
    intStrBUFF = (char*) calloc(size,sizeof(char));
    sprintf(intStrBUFF, "%lu", numConvert);
}

//Jesus man. Never used the EVP library at all
//https://wiki.openssl.org/index.php/EVP_Message_Digests
// + man 
void md4_init(void)
{
    /* 
    if (MD4_context != NULL)
        free(MD4_context);
    MD4_context = (MD4_CTX*) malloc(sizeof(MD4_CTX));
    */
    //md4_destruct();  
    SHOW("ALLOCATING CTX", "\n");
    ctx = (MD4_CTX*) malloc(sizeof(MD4_CTX));
    if (MD4_Init(ctx) == -1) EXIT_DESTROY(MD4_FAILURE, "MD4 FAILED TO INITIALIZE");
}

void md4_destruct(void)
{
    for (int i = 0; i < MD4_DIGEST_STRING_LENGTH; ++i) 
    {
        NEWLINE();
        SHOWI("VALUE OF HASH BEFORE: ",MD4_hash[i]);
        MD4_hash[i] = 0;
    }
    NEWLINE();
    //EVP_message_len = 0;
    //free = destroy, with backwards compat
    if (ctx != NULL)
    {
        SHOW(" FREEING CTX", "\n");
        ctx = easy_free(ctx);
    }
}

//DOES NOT OPEN OR CLOSE
void read_write(void)
{
    //ASSUMED IN AND OUT CHECK
    int read_count, write_count;
    read_count = write_count = 0;
    md4_init();

    //**EVP */
    //if (EVP_DigestInit_ex(EVP_context, EVP_md4(), NULL) != 1)
        //EXIT_DESTROY(MD4_ERROR, "Something bad happened when initializing, idk!");

    if (outBUFF == NULL)
        outBUFF = (char*) malloc(RDWR_BUFF_SIZE);
    
    while (!EOF_REACHED)
    {
        read_count = read(IN,outBUFF,RDWR_BUFF_SIZE);
        if (read_count == -1) EXIT_DESTROY(READ_FAIL, "failed to read");
        else if (read_count == 0) 
        {
            SAY("END OF FILE REACHED");
            EOF_REACHED = true;
        } else {
            write_count = write(OUT,outBUFF,read_count);
            if (write_count == -1) EXIT_DESTROY(WRITE_FAIL, "failed to write out to fd");

            //**EVP */
            if (MD4_Update(ctx, outBUFF, read_count) != 1) EXIT_DESTROY(MD4_ERROR, "Error when updating EVP digest");
        }
    }
    EOF_REACHED = false;
    
    //**EVP */
    if ((MD4_Final(MD4_hash, ctx) != 1)) EXIT_DESTROY(MD4_ERROR, "Error when finalizing hash? EVP SUCKS! (not true)");
    //FLUSH
    outBUFF = easy_free(outBUFF);
    md4_destruct();
}

void getStat(char* path)
{
    int stat_error;
    if (statHold != NULL)
        free(statHold);
    statHold = (struct stat*) malloc(sizeof(struct stat));
    stat_error = lstat(path, statHold);
    if (stat_error == -1) EXIT_DESTROY(READ_FAIL, "stat failed");
}

void getFileName(void)
{
    bool containsDelimit;
    int i;
    i = true;
    (containsDelimit) = 1;
    if (currentFileName != NULL) free(currentFileName);
    /*
    currentFileName = (char*) calloc(ARVIK_NAME_LEN, sizeof(char));
    currentFileName = strncpy(currentFileName, header->arvik_name, ARVIK_NAME_LEN);
    */

    while (!containsDelimit && i < ARVIK_NAME_LEN)
    {
        if(header->arvik_name[i] == ARVIK_NAME_TERM) containsDelimit = true;
        else ++i;
    }
    
    if (containsDelimit)
    {
        currentFileName = (char*) calloc(i + 1, sizeof(char));
        currentFileName = strncpy(currentFileName, header->arvik_name, i);
    }
}

void decipherHead(char line1[])
{
    char* currentPlace;
    char* sizeHolder;

    if (header != NULL) free(header);
    header = (my_arvik_header_t*) malloc(sizeof(my_arvik_header_t));
    sizeHolder = (char*) calloc(ARVIK_SIZE_LEN + 1, sizeof(char));

    //LENGTH CHECK
    if (strnlen(line1, ARVIK_HEADER_LEN) != ARVIK_HEADER_LEN) EXIT_DESTROY(READ_FAIL, "HEADER is not right size");

    //NAME
    if (allZeros( (unsigned char*) line1, ARVIK_NAME_LEN)) EXIT_DESTROY(NO_ARCHIVE_NAME, "Name is all spaces :/");
    /*header->arvik_name = */strncpy(header->arvik_name, line1, ARVIK_NAME_LEN);
    //sets file name to name with null terminated string, removes <
    getFileName();
    currentPlace = line1 + ARVIK_NAME_LEN;
    
    //DATE
    if (allZeros((unsigned char*) currentPlace, ARVIK_DATE_LEN) || notAllNumOrSpace(currentPlace, ARVIK_DATE_LEN)) EXIT_DESTROY(BAD_DATE, "DATE EITHER CONTAINS alphabet OR empty");
    /*header->arvik_date = */strncpy(header->arvik_date, currentPlace, ARVIK_DATE_LEN);
    currentPlace += ARVIK_DATE_LEN;

    //UID
    if (allZeros((unsigned char*) currentPlace, ARVIK_UID_LEN) || notAllNumOrSpace(currentPlace, ARVIK_UID_LEN)) EXIT_DESTROY(BAD_UID, "UID EITHER HAS EMPTY OR NON-NUM");
    strncpy(header->arvik_uid, currentPlace, ARVIK_UID_LEN);
    currentPlace += ARVIK_UID_LEN;

    //GID
    if (allZeros((unsigned char*) currentPlace, ARVIK_GID_LEN) || notAllNumOrSpace(currentPlace, ARVIK_GID_LEN)) EXIT_DESTROY(BAD_GID, "GID EITHER HAS EMPTY OR NON-NUM");
    strncpy(header->arvik_gid, currentPlace, ARVIK_GID_LEN);
    currentPlace += ARVIK_GID_LEN;

    //MODE
    if (allZeros((unsigned char*) currentPlace, ARVIK_MODE_LEN) || notAllNumOrSpace(currentPlace, ARVIK_MODE_LEN)) EXIT_DESTROY(BAD_MODE, "MODE EITHER HAS EMPTY OR NON-NUM");
    strncpy(header->arvik_mode, currentPlace, ARVIK_MODE_LEN);
    currentPlace += ARVIK_MODE_LEN;

    //SIZE
    if (allZeros((unsigned char*) currentPlace, ARVIK_SIZE_LEN) || notAllNumOrSpace(currentPlace, ARVIK_SIZE_LEN)) EXIT_DESTROY(BAD_SIZE, "SIZE EITHER HAS EMPTY OR NON-NUM");
    strncpy(header->arvik_size, currentPlace, ARVIK_SIZE_LEN);
    currentPlace += ARVIK_SIZE_LEN;

    if (*currentPlace != line1[ARVIK_HEADER_LEN - 3]) 
    {
        SHOW("KINDA MY BAD? CURRENT PLACE: ", currentPlace);
        NEWLINE();
        SHOW("\tWHERE WE SHOULD BE", line1 + ARVIK_HEADER_LEN - 3);
        NEWLINE();
        SHOW("FULL: ", line1);
        NEWLINE();
        EXIT_DESTROY(UNKNOWN_FAIL, __func__);
    }

    if (currentPlace[0] != ARVIK_TERM_BEGIN || currentPlace[1] != ARVIK_TERM_END) EXIT_DESTROY(BAD_TERM, "HEADER DID NOT TERMINATE PROPERLY");
    header->arvik_term[0] = ARVIK_TERM_BEGIN;
    header->arvik_term[1] = ARVIK_TERM_END;
    header->arvik_term[2] = '\0';

    md4_init();

   //**EVP */
    //if (EVP_DigestInit_ex(EVP_context, EVP_md4(), NULL) != 1)
        //EXIT_DESTROY(MD4_ERROR, "Something bad happened when initializing, idk!");

    //**EVP */
    if (MD4_Update(ctx, line1, ARVIK_HEADER_LEN) != 1) EXIT_DESTROY(MD4_ERROR, "Error when updating EVP digest");

    //**EVP */
    if ((MD4_Final(MD4_hash, ctx) != 1)) EXIT_DESTROY(MD4_ERROR, "Error when finalizing hash? EVP SUCKS! (not true)");
    INITARRAY(EXTRACT_GUESS_HEADER_HASH, MD4_HASH_MAX_LEN);
    strncpy(EXTRACT_GUESS_HEADER_HASH, (char*) MD4_hash, MD4_HASH_MAX_LEN);

    sizeHolder = strncpy(sizeHolder, header->arvik_size, ARVIK_SIZE_LEN);
    EXTRACT_MAX_READ = EXTRACT_current_read = EXTRACT_total_read = 0;
    EXTRACT_MAX_READ = atoi(sizeHolder);
    sizeHolder = easy_free(sizeHolder);

    md4_destruct();
}

void decipherFoot(char line[])
{
    char TheirHash[ARVIK_FOOTER_LEN + 1];
    char OurHash[ARVIK_FOOTER_LEN + 1];

    //if (MD4_hash_len == NULL || *MD4_hash_len == 0) EXIT_DESTROY(UNKNOWN_FAIL, __func__);

    TheirHash[ARVIK_FOOTER_LEN] = '\0';
    OurHash[ARVIK_FOOTER_LEN] = '\0';

    INITARRAY(TheirHash, ARVIK_FOOTER_LEN);
    INITARRAY(OurHash, ARVIK_FOOTER_LEN);

    //HEADER HASH?
    strncpy(TheirHash, line, MD4_HASH_MAX_LEN);
    strncpy(OurHash, EXTRACT_GUESS_HEADER_HASH, MD4_HASH_MAX_LEN);
    if (strcmp(OurHash, TheirHash) != 0) EXIT_DESTROY(MD4_ERROR, "HEADER HASH DOES NOT MASH (match)");

    //DATA HASH?
    strncpy(TheirHash,line + MD4_HASH_MAX_LEN, MD4_HASH_MAX_LEN);
    strncpy(OurHash, (char*) MD4_hash, MD4_HASH_MAX_LEN);
    if (strcmp(OurHash, TheirHash) != 0) EXIT_DESTROY(MD4_ERROR, "DATA HASH DO NOT MASH (match)");

}

void change_meta_data(void)
{
    //https://rosettacode.org/wiki/File_modification_time#C
    time_t mtime;
    struct utimbuf newTime;
    mtime = atoi(header->arvik_date);
    newTime.modtime = mtime;
    if (utime(currentFileName,&newTime) < 0) EXIT_DESTROY(MTIME_FAIL, "Could not change to new time");
}


void setHead(char* path) 
{
    int nameLen;
    int currentI;
    my_arvik_header_t* buff;
    buff = header;
    nameLen = strlen(path);
    currentI = 0;
    getStat(path);
    if (arvikLINES != NULL)
        free(arvikLINES);
    arvikLINES = (arvik_lines_t*) malloc(sizeof(arvik_lines_t));
    if (buff != NULL)
        free(header);
    header = (my_arvik_header_t*) malloc(sizeof(my_arvik_header_t));
    buff = header;


    INITARRAY(buff->arvik_name, ARVIK_NAME_LEN);
    INITARRAY(buff->arvik_date, ARVIK_DATE_LEN);
    INITARRAY(buff->arvik_uid, ARVIK_UID_LEN);
    INITARRAY(buff->arvik_gid, ARVIK_GID_LEN);
    INITARRAY(buff->arvik_mode, ARVIK_MODE_LEN);
    INITARRAY(buff->arvik_size, ARVIK_SIZE_LEN);
    //INITARRAY(buff->arvik_term, ARVIK_TERM_LEN);
    buff->arvik_term[0] = ARVIK_TERM_BEGIN;
    buff->arvik_term[1] = ARVIK_TERM_END;
    buff->arvik_term[2] = '\n';



    
    //**name
    if (nameLen < ARVIK_NAME_LEN - 1)
    {
        buff->arvik_name[nameLen] = ARVIK_NAME_TERM;
        strncpy(buff->arvik_name, path, nameLen);
    }
    else 
    {
        strncpy(buff->arvik_name,path, ARVIK_NAME_LEN);
    }

    //**date 
    sprintf(buff->arvik_date, "%lu", statHold->st_mtim.tv_sec);
    //holder = &statHold->st_mtim.tv_sec;
    //intToStr();
    //strncpy(buff->arvik_date,intStrBUFF, intToStrSize_Len);

    //**UID , GID, MODE, SIZE
    sprintf(buff->arvik_uid, "%d", statHold->st_uid);
    SHOW("TAHFHAF", buff->arvik_uid);
    //holder = &statHold->st_uid;
    //intToStr();
    //strncpy(buff->arvik_uid, intStrBUFF,intToStrSize_Len);

    sprintf(buff->arvik_gid, "%d", statHold->st_gid);
    //holder = &statHold->st_gid;
    //intToStr();
    //strncpy(buff->arvik_gid, intStrBUFF,intToStrSize_Len);

    sprintf(buff->arvik_mode, "%o", statHold->st_mode);
    //holder = &statHold->st_mode;
    //intToStr();
    //strncpy(buff->arvik_mode, intStrBUFF,intToStrSize_Len);
    
    sprintf(buff->arvik_size, "%lu", statHold->st_size);
    //intToStr();
    //strncpy(buff->arvik_size, intStrBUFF,intToStrSize_Len);

    holder = NULL;
    strncpy(arvikLINES->start, ARVIK_TAG, ARVIK_TAG_LEN + 1);
    currentI = ARVIK_NAME_LEN;
    strncpy(arvikLINES->header + currentI, buff->arvik_date,ARVIK_DATE_LEN);
    currentI += ARVIK_DATE_LEN;
    strncpy(arvikLINES->header + currentI,buff->arvik_uid, ARVIK_UID_LEN);
    currentI += ARVIK_UID_LEN;
    strncpy(arvikLINES->header + currentI,buff->arvik_gid, ARVIK_GID_LEN);
    currentI += ARVIK_GID_LEN;
    strncpy(arvikLINES->header + currentI,buff->arvik_mode, ARVIK_MODE_LEN);
    currentI += ARVIK_MODE_LEN;
    strncpy(arvikLINES->header + currentI,buff->arvik_size, ARVIK_SIZE_LEN);
    currentI += ARVIK_SIZE_LEN;
    strncpy(arvikLINES->header + currentI,buff->arvik_term, ARVIK_TERM_LEN);
    currentI += ARVIK_TERM_LEN;

    if(create)
    {
        write(OUT, buff->arvik_name, ARVIK_NAME_LEN);
        write(OUT, buff->arvik_date, ARVIK_DATE_LEN);
        write(OUT, buff->arvik_uid, ARVIK_UID_LEN);
        //write(OUT, " ", 1);
        write(OUT, buff->arvik_gid, ARVIK_GID_LEN);
        //write(OUT, " ", 1);
        write(OUT, buff->arvik_mode, ARVIK_MODE_LEN);
        //write(OUT, " ", 1);
        write(OUT, buff->arvik_size, ARVIK_SIZE_LEN);
        write(OUT, buff->arvik_term, ARVIK_TERM_LEN);
    }
    
}

void startFoot(void)
{
    //const EVP_MD *md4_digest = EVP_md4();
    GATEPASS(304);
    if (arvikLINES == NULL) EXIT_DESTROY(UNKNOWN_FAIL, __func__);
    //if (EVP_context == NULL || allZeros(MD4_hash, MD4_HASH_MAX_LEN)) EXIT_DESTROY(MD4_FAIL, __func__);
    //if (footer == NULL) EXIT_DESTROY(READ_FAIL, "Footer not initialized by set foot");
    if (footer != NULL)
        free(footer);
    footer = (my_arvik_footer_t*) malloc(sizeof(my_arvik_footer_t));
    INITARRAY(footer->md4sum_data, MD4_HASH_MAX_LEN);
    INITARRAY(footer->md4sum_header, MD4_HASH_MAX_LEN);
    md4_init();
    GATEPASS(305);

    //**EVP */
    //if (EVP_DigestInit_ex(EVP_context, md4_digest, NULL) != 1)
        //EXIT_DESTROY(MD4_ERROR, "Something bad happened when initializing, idk!");
    GATEPASS(306);
    

    //**EVP */
    if (MD4_Update(ctx, arvikLINES->header, ARVIK_HEADER_LEN) != 1) EXIT_DESTROY(MD4_ERROR, "Error when updating EVP digest");
    if ((MD4_Final(MD4_hash, ctx) != 1)) EXIT_DESTROY(MD4_ERROR, "Error when finalizing hash? EVP SUCKS! (not true)");
    GATEPASS(307);


    footer->arvik_term[0] = ARVIK_TERM_BEGIN;
    GATEPASS(308);
    footer->arvik_term[1] = ARVIK_TERM_END;
    GATEPASS(3009);
    footer->arvik_term[2] = '\0';
    GATEPASS(3010);

    strncpy(( footer->md4sum_header), (char* )MD4_hash, MD4_HASH_MAX_LEN);
    GATEPASS(3019);
    GATEPASS(309);
}

void endFoot(void)
{
    char* pointerToNext;
    GATEPASS(311);
    if (ctx == NULL) EXIT_DESTROY(MD4_ERROR, "Something went wrong here, EVP hash is empty at end of footer init");
    if (footer == NULL) EXIT_DESTROY(UNKNOWN_FAIL, __func__);
    if (arvikLINES == NULL) EXIT_DESTROY(UNKNOWN_FAIL, __func__);

    strncpy(footer->md4sum_data, (char*) MD4_hash, MD4_HASH_MAX_LEN);
    md4_destruct();
    
    //ALL TOGETHER NOW!
    GATEPASS(312);
    strncpy(arvikLINES->footer, footer->md4sum_header ,MD4_HASH_MAX_LEN);
    pointerToNext = arvikLINES->footer + MD4_HASH_MAX_LEN;
    pointerToNext = strncpy(pointerToNext, footer->md4sum_data, MD4_HASH_MAX_LEN);
    pointerToNext = pointerToNext + MD4_HASH_MAX_LEN;
    pointerToNext[0] = ARVIK_TERM_BEGIN;
    pointerToNext[1] = ARVIK_TERM_END;
    pointerToNext[2] = '\0';

    header = easy_free(header);
    footer = easy_free(footer);
    GATEPASS(319);
}

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
    GATEPASS(799);
}
void TOC_SIMPLE(void)
{
    int toMove;
    int times;
    toMove = 0;
    times = 1;
    readError = 1;
    while (readError != 0)
    {
        if (EXTRACT_header != NULL) free(EXTRACT_header);
        if (EXTRACT_footer != NULL) free(EXTRACT_footer);
        EXTRACT_header = (char*) calloc(ARVIK_HEADER_LEN + 1, sizeof(char));
        EXTRACT_footer = (char*) calloc(ARVIK_FOOTER_LEN + 1, sizeof(char));

        readError = read(IN_TOC, EXTRACT_header, ARVIK_HEADER_LEN);
        if (readError == -1) EXIT_DESTROY(TOC_ERROR_BEGIN, "Could not read the file to extract TOC"); 
        EOFCHECK(readError);
        
        decipherHead(EXTRACT_header);
        toMove += atoi(header->arvik_size);
        lseek(IN_TOC, ((ARVIK_HEADER_LEN + ARVIK_FOOTER_LEN) * times) + toMove + ARVIK_HEADER_LEN , SEEK_SET); 
        ++times;
        printf("%s\n", currentFileName);
    }

}

toc_verify_t* TOC_VERIFY_STRUCT_INIT(toc_verify_t* outP, bool verbosed)
{
    if (outP != NULL) free(outP);
    outP = (toc_verify_t*) malloc(sizeof(toc_verify_t));
    
    outP->IS_VERBOSE = verbosed;
    outP->titleISUnder = false;
    INITARRAY(outP->titleUnderStr, TOC_VERIFY_TITLE_UNDER_TOTAL_LEN);

    //strncpy();

    INITARRAY(outP->titleOverStr, TOC_VERIFY_TITLE_OVER_TOTAL_LEN);
    outP->header_t.headerPass = false;
    outP->header_t.pass = VERIFY_HEADER_FAIL;
    //outP->header_t.content

    outP->data_t.dataPass = false;
    outP->data_t.pass = VERIFY_DATA_FAIL;

    return outP;
}

void TOC_HASH(bool verboseTrigger, bool verifyTrigger)
{
    if (verifyTrigger)
    {
        if (strlen(currentFileName) < TOC_VERIFY_TITLE_UNDER_LEN)
        {
            printf("%s", currentFileName);
            SPACEPRINT((TOC_VERIFY_TITLE_UNDER_TOTAL_LEN - 1 - strlen(currentFileName)));
            printf("\n");
        } else {
            printf("%s", currentFileName);
            SPACEPRINT((TOC_VERIFY_TITLE_OVER_TOTAL_LEN - 1 - strlen(currentFileName)));
            printf("\n");
        }
        if (HEAD_MD4_FAILURE)
        {
            MD4_FAILURE = HEAD_MD4_FAILURE;
            printf("%s", VERBOSE_VERIFY_HEADER_MD4_CONTENT);
            printf("%32s", EXTRACT_GUESS_HEADER_HASH);
            printf("\n");
        } else 
        {
            printf(VERIFY_HEADER_SUCCESS);
            if (verboseTrigger)
                printf("%32s", EXTRACT_GUESS_HEADER_HASH);
        }
        
        if (FOOT_MD4_FAILURE)
        {
            MD4_FAILURE = FOOT_MD4_FAILURE;
            printf("%s", VERBOSE_VERIFY_HEADER_MD4_FOOTER);
            printf("%32s", MD4_hash);
            printf("\n");
        } else 
        {
            printf(VERIFY_HEADER_SUCCESS);
            if (verboseTrigger)
                printf("%32s", EXTRACT_GUESS_HEADER_HASH);
        }
    }
    else if (verboseTrigger)
    {
        printf("%s%s\n", VERBOSE_HEADER_MD4, EXTRACT_GUESS_HEADER_HASH);
        printf("%s%s\n", VERBOSE_HEADER_MD4, EXTRACT_GUESS_HEADER_HASH);
    }
    else 
    {
        EXIT_DESTROY(UNKNOWN_FAIL, __func__);
    }
    

    //** !!!IMPLEMENT!!! */
}

void TOC_VERIFY(void)
{
    toc_verify_t* outPrint [[maybe_unused]];
    int toMove;
    int times;
    readError = 1;
    while (readError != 0)
    {
        if (EXTRACT_header != NULL) free(EXTRACT_header);
        if (EXTRACT_footer != NULL) free(EXTRACT_footer);
        EXTRACT_header = (char*) calloc(ARVIK_HEADER_LEN + 1, sizeof(char));
        EXTRACT_footer = (char*) calloc(ARVIK_FOOTER_LEN + 1, sizeof(char));

        readError = read(IN_TOC, EXTRACT_header, ARVIK_HEADER_LEN);
        if (readError == -1) EXIT_DESTROY(TOC_ERROR_BEGIN, "Could not read the file to extract TOC"); 
        EOFCHECK(readError);
        
        decipherHead(EXTRACT_header);
        toMove += atoi(header->arvik_size);
        lseek(IN_TOC, toMove, SEEK_CUR);
        readError = read(IN_TOC, EXTRACT_footer, ARVIK_FOOTER_LEN);
        if (readError == -1) EXIT_DESTROY(READ_FAIL, "FAILED TO READ FOOTER IN TOC");
        decipherFoot(EXTRACT_footer);
        ++times;

        TOC_HASH(false,true);

        //** VERIFY HEADER FAILURE */
        //** VERIFY DATA FAILURE */

    }

}

void TOC_VERBOSE(void)
{
    struct passwd* pwd;
    struct group* grp;
    int toMove;
    int times;
    int octal;
    char octalVals[9] = {'-','-','-','-','-','-','-','-','-'};
    pwd = NULL;
    grp = NULL;
    toMove = 0;
    times = 1;
    octal = 0;
    readError = 1;
    while (readError != 0)
    {
        if (EXTRACT_header != NULL) free(EXTRACT_header);
        if (EXTRACT_footer != NULL) free(EXTRACT_footer);
        EXTRACT_header = (char*) calloc(ARVIK_HEADER_LEN + 1, sizeof(char));
        EXTRACT_footer = (char*) calloc(ARVIK_FOOTER_LEN + 1, sizeof(char));

        readError = read(IN_TOC, EXTRACT_header, ARVIK_HEADER_LEN);
        if (readError == -1) EXIT_DESTROY(TOC_ERROR_BEGIN, "Could not read the file to extract TOC"); 
        EOFCHECK(readError);
        
        decipherHead(EXTRACT_header);
        toMove += atoi(header->arvik_size);
        lseek(IN_TOC, ((ARVIK_HEADER_LEN + ARVIK_FOOTER_LEN) * times) + toMove + ARVIK_HEADER_LEN , SEEK_SET); 
        ++times;
        
        //line1
        printf("%s\n", currentFileName);
        printf("%s", VERBOSE_MODE);
        

        for (int i = 0; i < ARVIK_MODE_LEN; ++i)
        {
            switch(header->arvik_mode[i])
            {
            case '1':
                octalVals[octal] = '-';
                ++octal;
                octalVals[octal] = '-';
                ++octal;
                octalVals[octal] = 'x';
                ++octal;
                break;
            case '2':
                octalVals[octal] = '-';
                ++octal;
                octalVals[octal] = 'w';
                ++octal;
                octalVals[octal] = '-';
                ++octal;
                break;
            case '3':
                octalVals[octal] = '-';
                ++octal;
                octalVals[octal] = 'w';
                ++octal;
                octalVals[octal] = 'x';
                ++octal;
                break;
            case '4':
                octalVals[octal] = 'r';
                ++octal;
                octalVals[octal] = '-';
                ++octal;
                octalVals[octal] = '-';
                ++octal;
                break;
            case '5':
                octalVals[octal] = 'r';
                ++octal;
                octalVals[octal] = '-';
                ++octal;
                octalVals[octal] = 'x';
                ++octal;
                break;
            case '6':
                octalVals[octal] = 'r';
                ++octal;
                octalVals[octal] = 'w';
                ++octal;
                octalVals[octal] = '-';
                ++octal;
                break;
            case '7':
                octalVals[octal] = 'r';
                ++octal;
                octalVals[octal] = 'w';
                ++octal;
                octalVals[octal] = 'x';
                ++octal;
                break;
            default:
                continue;
            }
        }
        //print mode
        printf("%s\n",octalVals);



        pwd = getpwuid(atoi(header->arvik_uid));
        grp = getgrgid(atoi(header->arvik_gid));


        //GID && UID
        printf("%s%12s%s\n", VERBOSE_UID,header->arvik_uid, pwd->pw_name);
        printf("%s%12s%s\n", VERBOSE_GID, header->arvik_gid, grp->gr_name);

        if (VERBOSE_TIME_STRUCT != NULL) free(VERBOSE_TIME_STRUCT);
        //VERBOSE_TIME_STRUCT = (struct tm*) malloc(sizeof(struct tm));
        if ((VERBOSE_TIME_STRUCT = localtime( ((time_t*) atol(header->arvik_date)))) == NULL)
            EXIT_DESTROY(TOC_ERROR_TIME, "TOC FAILED TO ASSIGN TIME?");
        if ((strftime(VERBOSE_TIME_RESULT, TOC_MTIME_RESULT_LEN,VERBOSE_MTIME_FORMAT, VERBOSE_TIME_STRUCT)) == 0)
            EXIT_DESTROY(TOC_ERROR_TIME, "TOC FAILED TO GET TIME STRING?");
        //MTIME
        printf("%s      %s\n", VERBOSE_MTIME, VERBOSE_TIME_RESULT);

        //header && footer
        TOC_HASH(true,false);
    }


}

void TOC_VERIFY_VERBOSE(void)
{
    toc_verify_t* outPrint [[maybe_unused]];
    int toMove;
    int times;
    readError = 1;
    while (readError != 0)
    {
        if (EXTRACT_header != NULL) free(EXTRACT_header);
        if (EXTRACT_footer != NULL) free(EXTRACT_footer);
        EXTRACT_header = (char*) calloc(ARVIK_HEADER_LEN + 1, sizeof(char));
        EXTRACT_footer = (char*) calloc(ARVIK_FOOTER_LEN + 1, sizeof(char));

        readError = read(IN_TOC, EXTRACT_header, ARVIK_HEADER_LEN);
        if (readError == -1) EXIT_DESTROY(TOC_ERROR_BEGIN, "Could not read the file to extract TOC"); 
        EOFCHECK(readError);
        
        decipherHead(EXTRACT_header);
        toMove += atoi(header->arvik_size);
        lseek(IN_TOC, toMove, SEEK_CUR);
        readError = read(IN_TOC, EXTRACT_footer, ARVIK_FOOTER_LEN);
        if (readError == -1) EXIT_DESTROY(READ_FAIL, "FAILED TO READ FOOTER IN TOC");
        decipherFoot(EXTRACT_footer);
        ++times;
        TOC_HASH(true,true);
    }
    if (MD4_FAILURE)
        printf("%s", VERBOSE_VERIFY_FAILURE);
}

void* easy_free(void* this)
{
    if (this == NULL) return NULL;
    free (this);
    return NULL;
}

void EXIT_DESTROY(int error, const char* msg)
{
    GATEPASS(-900);
    close(OUT);
    close(IN);
    if (out_string != NULL)
    {
        GATEPASS(-901);
        SHOW("OUT STRING WAS NOT EMPTY", out_string);
        NEWLINE();
        out_string = easy_free(out_string);
    }
    if (VERBOSE_TIME_STRUCT != NULL)
    {
        GATEPASS(-902);
        SHOW("VERBOSE TIME STRUCT WAS NOT EMPTY", "\n");
        easy_free(VERBOSE_TIME_STRUCT);
    }
    if (holder != NULL)
    {
        GATEPASS(-903);
        SHOW("BAD EXIT", "HOLDER WAS NOT NULL (cannot show)");
        NEWLINE();
        holder = easy_free(holder);
    }
    /*
    if (fileV_INT != NULL)
    {
        SHOW("BAD EXIT", "fileV_INT was NOT NULL\t");
        SHOWI("fileV_INT value: ", *fileV_INT);
        NEWLINE();
        fileV_INT = easy_free(fileV_INT);
    }*/
    if (currentARG != NULL)
    {
        GATEPASS(-904);
        currentARG = (char*) currentARG;
        SHOW("BAD EXIT", "currentARG was NOT NULL\t");
        SHOW("currentArg: ", currentARG);
        NEWLINE();
        GATEPASS(-941);
        //currentARG = easy_free(currentARG);
        GATEPASS(-942);
    }
    if (intStrBUFF != NULL)
    {
        GATEPASS(-905);
        SHOW("BAD EXIT", "intStrBUFF was NOT NULL\t");
        SHOWI("intStrBUFF[0]: ", *intStrBUFF);
        NEWLINE();
        intStrBUFF = easy_free(intStrBUFF);
    }
    if (inBUFF != NULL)
    {
        GATEPASS(-906);
        SHOW("IN BUFF WAS NOT NULL: ", inBUFF);
        NEWLINE();
        inBUFF = easy_free(inBUFF);
    }
    if (outBUFF != NULL)
    {
        GATEPASS(-907);
        SHOW("OUT BUFF WAS NOT NULL: ", outBUFF);
        NEWLINE()
        outBUFF = easy_free(outBUFF);
    }
    SHOW("pre stathold", "\n");
    if ((struct stat*) statHold != NULL)
    {
        GATEPASS(-908);
        SHOW("BAD EXIT", "statHold was NOT NULL (cannot show)");
        NEWLINE();
        statHold = easy_free(statHold);
    }
    if (header != NULL)
    {
        GATEPASS(-909);
        SHOW("BAD EXIT", "header was NOT NULL (cannot show)");
        NEWLINE();
        header = easy_free(header);
    }
    if (footer != NULL)
    {
        GATEPASS(-910);
        SHOW("BAD EXIT", "footer was NOT NULL (cannot show)");
        NEWLINE();
        footer = easy_free(footer);
    }
    if (verboseDATA != NULL)
    {
        GATEPASS(-911);
        SHOW("BAD EXIT", "verboseDATA was NOT NULL (cannot show)");
        NEWLINE();
        verboseDATA = easy_free(verboseDATA);
    }
    if (verifyDATA != NULL)
    {
        GATEPASS(-912);
        SHOW("BAD EXIT", "verifyDATA was NOT NULL (cannot show)");
        NEWLINE();
        verifyDATA = easy_free(verifyDATA);
    }
    if (arvikLINES != NULL)
    {
        SHOW("BAD EXIT", "arvikLines was NOT NULL (cannot show)");
        NEWLINE();
        arvikLINES = easy_free(arvikLINES);
    }

    if (ctx != NULL)
    {
        SHOW("OH YOU REALLY SCREWED IT, EVP NOT NULL",  "(cannot show)");
        NEWLINE();
        md4_destruct();
        //EVP_MD_CTX_free(EVP_context);
        //EVP_context = NULL;
    }
    if (EXTRACT_header != NULL)
    {
        SHOW("EXTRACT HEADER WAS NOT EMPTY: ", EXTRACT_header);
        NEWLINE();
        EXTRACT_header = easy_free(EXTRACT_header);
    }
    if (EXTRACT_footer != NULL)
    {
        SHOW("EXTRACT FOOTER WAS NOT EMPTY: ", EXTRACT_footer);
        NEWLINE();
        EXTRACT_footer = easy_free(EXTRACT_footer);
    }
    if (currentFileName != NULL)
    {
        SHOW("CURRENT FILE NAME WAS NOT EMPTY: ", currentFileName);
        NEWLINE();
        currentFileName = easy_free(currentFileName);
    }



    perror(msg);
    if (error == UNKNOWN_FAIL)
        fprintf(stderr, "SOMETHING BAD HAPPENED IN %s\n", msg);
    else
        fprintf(stderr, "%s\n", msg);
    exit(error);
}


//**!-----------------------------------------MAIN------------------------------------*/
int main(int argc, char**argv)
{
    char c;
    int argIn [[maybe_unused]];


    int statStatus [[maybe_unused]];
    statStatus = 0;
    //OpenSSL_add_all_digests();

    fprintf(stderr, "Value of argc: %d\n", argc);
    IN = STDIN_FILENO;
    OUT = STDOUT_FILENO;

    statHold = (struct stat*) malloc(sizeof(struct stat));

    for (int i = 0; i < argc; ++i)
    {
        SHOWI("arg", i);
        SHOW(": ", argv[i]);
        NEWLINE();
    }
    SHOWI("\nargc: ", argCount);
    fprintf(stderr, "Value of argc: %d\n", argc);

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
                out_string = strdup(optarg);
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
            case '?':
                perror("UNKNOWN COMMAND LINE ARG");
        }
        SHOWI("\nargc: ", argCount);
    }
    GATEPASS(1); //past options
    
    fileC = argc - (optind);
    argCount = 0;
    SHOWI("\nargc: ", argc);
    SHOWI("\nargc: ", argCount);
    SHOWI("\nfileC", fileC);
    SHOWI("\noptind", optind);
    if (argc == 1) 
    {
        printf("*** ./arvik-md4 No action specified\n");
        EXIT_DESTROY(NO_ACTION_GIVEN, "NO ARGS");
    }
    //argIn = optind;
    if (!help)
    {
        GATEPASS(700);
        if (filename)
        {
            GATEPASS(701);
            if (create)
            {
                //file exist: https://stackoverflow.com/a/230068 

                GATEPASS(720);
                OUT = open(out_string, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH );
                if (OUT == -1) EXIT_DESTROY(WRITE_FAIL, "COULDN'T OPEN FILE");
                IN_TOC = OUT;
            } else //extract
            { 
                IN = open(out_string, O_RDONLY);
                IN_TOC = open(out_string, O_RDONLY);
            }
        } else {
            GATEPASS(730);
            IN = STDIN_FILENO;
            IN_TOC = STDIN_FILENO;
        }
        GATEPASS(799);

        if (extract)
        {
            GATEPASS(200);
            extract_arvik();
            /*
            if (inBUFF != NULL) free(inBUFF);
            inBUFF = (char*) malloc(RDWR_BUFF_SIZE);

            while (readError != 0)
            {
                if (EXTRACT_header != NULL) free(EXTRACT_header);
                if (EXTRACT_footer != NULL) free(EXTRACT_footer);
                EXTRACT_header = (char*) calloc(ARVIK_HEADER_LEN + 1, sizeof(char));
                EXTRACT_footer = (char*) calloc(ARVIK_FOOTER_LEN + 1, sizeof(char));

                //READ TAG
                if (readError == -1) EXIT_DESTROY(READ_FAIL, "Something happened in process of extracting, failed");
                readError = read(IN, inBUFF, ARVIK_TAG_LEN);
                if (readError == -1) EXIT_DESTROY(READ_FAIL, "failed to read tag in extract");
                if (strncmp(inBUFF, ARVIK_TAG, ARVIK_TAG_LEN) != 0) EXIT_DESTROY(BAD_TAG, "Checked tag and it did not match");
                GATEPASS(201);

                //DECIPHER HEADER
                readError = read(IN, EXTRACT_header, ARVIK_HEADER_LEN);
                if (readError == -1) EXIT_DESTROY(READ_FAIL, "Failed to read header");
                decipherHead(EXTRACT_header);
                GATEPASS(202);
                
                //Open/ Create File
                if (access(currentFileName, F_OK))
                {
                    OUT = open(currentFileName, O_WRONLY | O_TRUNC);
                } else {
                    OUT = open(currentFileName, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH |O_CREAT | O_WRONLY);
                }

                //verbose
                if (verbose) printf("\nx- %s", currentFileName);

                //START OUR MD4 DATA ANALYSIS
                md4_init();
                //if (EVP_DigestInit_ex(EVP_context, EVP_md4(), NULL) != 1)
                    //EXIT_DESTROY(MD4_ERROR, "Something bad happened when initializing, idk!");

                //GET OUT THAT DATA!!!
                for(EXTRACT_total_read = 0; EXTRACT_total_read < EXTRACT_MAX_READ; EXTRACT_total_read += EXTRACT_current_read)
                {
                    EXTRACT_current_read = read(IN,inBUFF,RDWR_BUFF_SIZE);
                    if (EXTRACT_current_read == -1) EXIT_DESTROY(EXTRACT_FAIL, "FAILED TO READ IN EXTRACT");
                    if (EXTRACT_current_read == 0) EXIT_DESTROY(EXTRACT_FAIL, "REACHED EOF EARLY");
                    writeError = write(OUT,inBUFF,EXTRACT_current_read);
                    if (MD4_Update(ctx, outBUFF, EXTRACT_current_read) != 1) EXIT_DESTROY(MD4_ERROR, "Error when updating EVP digest");
                }

                //COMPARE OURS WITH THEIRS
                if ((MD4_Final(MD4_hash, ctx) != 1)) EXIT_DESTROY(MD4_ERROR, "Error when finalizing hash? EVP SUCKS! (not true)");
                readError = read(IN, EXTRACT_footer, ARVIK_FOOTER_LEN);
                if (readError == -1) EXIT_DESTROY(READ_FAIL, "FAILED TRYING TO READ FOOTER");
                decipherFoot(EXTRACT_footer);
                
                //CHANGE THE MODIFY DATE
                change_meta_data();
                md4_destruct();
                close(OUT);
            }
            */
            //DECIPHER DATA

            GATEPASS(299);
        }
        GATEPASS(2);

        if (create)
        {

            GATEPASS(301);
            writeError = write(OUT, ARVIK_TAG, ARVIK_TAG_LEN);
            if (writeError == -1) EXIT_DESTROY(CREATE_FAIL, "failed to write the tag?");
            if (fileC > 0)
            {
                create_arvik(argv, argc, optind);
                /*
                for (argIn = optind; argIn < argc ; ++argIn)
                {
                    SHOWI("\nIN CREATE FOR: ", argIn);
                    currentARG = argv[argIn];

                    GATEPASS(302);
                    //setHead(currentARG);
                    GATEPASS(303);
                    //startFoot();
                    GATEPASS(304);

                    if (verbose)
                        printf("a - %s\n", currentARG);

                    IN = open(currentARG, O_RDONLY);
                    if (IN == -1) EXIT_DESTROY(OPEN_FAIL, "failed to open file");
                    writeError = write(OUT,arvikLINES->header, ARVIK_HEADER_LEN);
                    if (writeError == -1) EXIT_DESTROY(WRITE_FAIL, "Failed to write header");
                    read_write();
                    //endFoot();
                    writeError = write(OUT, arvikLINES->footer, ARVIK_FOOTER_LEN);
                    if (writeError == -1) EXIT_DESTROY(WRITE_FAIL, "Failed to write footer");
                    close(IN);
                }
                GATEPASS(310);
                for (int i = 0; i < argCount; ++i)
                {
                    //holdTheHolder = (struct stat*) malloc(sizeof(struct stat));
                    writeError = stat(currentARG, &holdTheHolder);
                    currentARG = argv[optind + i];
                    SHOW("IN FOR?", "\n");
                    if (writeError == -1)
                    {
                        SHOWI("error num", errno);
                        GATEPASS(319);
                        //holdTheHolder = easy_free(holdTheHolder);
                        write(STDOUT_FILENO,"could not open member file: No such file or directory\n", 55);
                        write(STDOUT_FILENO,"*** Cannot open member file ", 29); 
                        write(STDOUT_FILENO, currentARG, strlen(currentARG));
                        write(STDOUT_FILENO, "\n", 1);
                        EXIT_DESTROY(NO_ARCHIVE_NAME, "NOT FILE OR DIR");
                    }
                    currentARG = easy_free(currentARG);
                    //holdTheHolder = easy_free(holdTheHolder);
                }
                */
                //if (holder != NULL) statHold = (struct stat*) holder;
            }
            //close(OUT);
            GATEPASS(399);
        }
        GATEPASS(3);

        if (toc)
        {
            if (verify)
            {
                if (verbose) //**VERIFY && VERBOSE (v AND V)*/
                {
                    //EVIL!!!
                    TOC_VERIFY_VERBOSE();
                    GATEPASS(601);
                }
                else  //**VERIFY && !VERBOSE (v AND NOT V)*/
                {
                    //
                    TOC_VERIFY();
                    GATEPASS(501);
                }
            } else if (verbose) {//** !VERIFY && VERBOSE (NOT v AND V)*/

                TOC_VERBOSE();
                GATEPASS(401);

            } else{ //** !VERIFY && !VERBOSE (NOT v AND NOT V)*/
                TOC_SIMPLE();
                GATEPASS(301);
            }
        }
    } else {
        GATEPASS(801);
        displayHelp();
    }

    GATEPASS(999);
    EXIT_DESTROY(EXIT_SUCCESS, "GOOD JOB! EXITED SUCCESSFULLY");
}
#pragma GCC diagnostic push
#pragma GCC diagnostic pop