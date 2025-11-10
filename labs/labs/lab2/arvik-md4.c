//**-----------------------------------------INCLUDES ------------------------------------*/
#include "arvik.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>

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


//mtime
# define TOC_MTIME_TYPE_LEN 6
# define TOC_MTIME_SPACING 7
# define TOC_MTIME_MONTH_LEN 3
# define TOC_MTIME_MONTH_DAY_SPACING 2
# define TOC_MTIME_DAY_HOUR_SPACING 1
# define TOC_MTIME_COLON_INDEX 25
# define TOC_MTIME_MIN_YEAR_SPACING 1
# define TOC_MTIME_TOTAL_LEN 34

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
# define ARVIK_TAG_LEN 10
# define RDWR_BUFF_SIZE 10
# define SAY(str) fprintf(stderr, "%s\n", str)
# define INITARRAY(buff, len) for (int i = 0; i < len; ++i) buff[i] = ' '
# define WRITE_FAIL 11
# define OPEN_FAIL 12

//** OTHER DEFINES END      */

//** CREATE DEFINES BEGIN    */############################################
# define ARVIK_HEADER_LEN 78
# define ARVIK_FOOTER_LEN 66
//** CREATE DEFINES END      */

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
static int currentFD = 0;
static int IN = STDIN_FILENO;
static int OUT = STDOUT_FILENO;
static int writeError = 0;
static int intToStrSize_Len = 0;
static bool EOF_REACHED = false;

//const char begin[] = "#<arvik4>\n";


//toc-verbose
const char VERBOSE_FILE_NAME[] = "file name: ";
const char VERBOSE_MODE[] = "    mode:";
const char VERBOSE_UID[] = "    uid:";
const char VERBOSE_GID[] = "    gid:";
const char VERBOSE_SIZE[] = "    size:";
const char VERBOSE_MTIME[] = "    mtime:      MON  D HH:MM YEAR";
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
        char* pass;
        char* content;
        char* footer;
    } header_t;

    struct data_s{
        bool dataPass;
        char* pass;
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
static int* fileV_INT = NULL;
static char* currentARG = NULL;
static char* intStrBUFF = NULL;
static char* outBUFF = NULL;
static char** fileV_STR = NULL;
static struct stat* statHold = NULL;
static arvik_header_t* header = NULL;
static arvik_footer_t* footer = NULL;
static toc_verbose_t* verboseDATA = NULL;
static toc_verify_t* verifyDATA = NULL;
static arvik_lines_t* arvikLINES = NULL;

//**-----------------------------------------PROTOTYPES ------------------------------------*/

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

//**-----------------------------------------FUNCTIONS ------------------------------------*/
unsigned long intToStrSize()
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

void intToStr()
{
    unsigned long numConvert;
    unsigned long size;
    size = intToStrSize();
    intToStrSize_Len = size;
    numConvert = *((unsigned long*) holder);
    if (intStrBUFF != NULL)
        free(intStrBUFF);
    intStrBUFF = (char*) calloc(size,1);
    sprintf(intStrBUFF, "%lu", numConvert);
}
//DOES NOT OPEN OR CLOSE
void read_write()
{
    //ASSUMED IN AND OUT CHECK
    int read_count, write_count;
    read_count = write_count = 0;
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
        }
    }
    EOF_REACHED = false;
    //FLUSH
    outBUFF = easy_free(outBUFF);
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

void setHead(char* path) 
{
    int nameLen;
    int currentI;
    arvik_header_t* buff;
    buff = header;
    nameLen = strlen(path);
    currentI = 0;
    getStat(path);
    if (arvikLINES != NULL)
        free(arvikLINES);
    arvikLINES = (arvik_lines_t*) malloc(sizeof(arvik_lines_t));
    if (buff != NULL)
        free(header);
    header = (arvik_header_t*) malloc(sizeof(arvik_header_t));
    buff = header;


    INITARRAY(buff->arvik_name, ARVIK_NAME_LEN);
    INITARRAY(buff->arvik_date, ARVIK_DATE_LEN);
    INITARRAY(buff->arvik_uid, ARVIK_UID_LEN);
    INITARRAY(buff->arvik_gid, ARVIK_GID_LEN);
    INITARRAY(buff->arvik_mode, ARVIK_MODE_LEN);
    INITARRAY(buff->arvik_size, ARVIK_SIZE_LEN);
    //INITARRAY(buff->arvik_term, ARVIK_TERM_LEN);
    buff->arvik_term[0] = *ARVIK_TERM;
    buff->arvik_term[1] = '\n';


    
    //**name
    if (nameLen < ARVIK_NAME_LEN - 1)
    {
        buff->arvik_name[nameLen + 1] = ARVIK_NAME_TERM;
        strncpy(buff->arvik_name, path, nameLen);
    }
    else 
    {
        strncpy(buff->arvik_name,path, ARVIK_NAME_LEN);
    }

    //**date 
    holder = &statHold->st_mtim.tv_sec;
    intToStr();
    strncpy(buff->arvik_date,intStrBUFF, intToStrSize_Len);

    //**UID , GID, MODE, SIZE
    holder = &statHold->st_uid;
    intToStr();
    strncpy(buff->arvik_uid, intStrBUFF,intToStrSize_Len);

    holder = &statHold->st_gid;
    intToStr();
    strncpy(buff->arvik_gid, intStrBUFF,intToStrSize_Len);

    holder = &statHold->st_mode;
    intToStr();
    strncpy(buff->arvik_mode, intStrBUFF,intToStrSize_Len);
    
    holder = &statHold->st_size;
    intToStr();
    strncpy(buff->arvik_size, intStrBUFF,intToStrSize_Len);

    holder = NULL;
    strncpy(arvikLINES->start, ARVIK_TAG, ARVIK_TAG_LEN);
    strncpy(arvikLINES->header, buff->arvik_name, ARVIK_NAME_LEN);
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

}

void setFoot(char* path)
{

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

void tocSimple(int count, char**names, int fd)
{
    printf("\n");
    for (int i = 0; i < count; ++i)
    {
        for(int j = 0; j < ARVIK_NAME_LEN; ++j)
        {
            if (names[i][j] == ARVIK_NAME_TERM)
                continue;
            else
                printf("%c", names[i][j]);
        }
        printf("\n");
    }
}

void* easy_free(void* this)
{
    if (this == NULL) return NULL;
    free (this);
    return NULL;
}

void EXIT_DESTROY(int error, const char* msg)
{
    fileV_INT = easy_free(fileV_INT);
    switch(error)
    {

    }

}

//**!-----------------------------------------MAIN------------------------------------*/
int main(int argc, char**argv)
{
    char c;
    int argIn [[maybe_unused]];


    int statStatus, fileC = 0;

    IN = STDIN_FILENO;
    OUT = STDOUT_FILENO;

    statHold = (struct stat*) malloc(sizeof(struct stat));

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
    }
    GATEPASS(1); //past options
    
    argCount = fileC = argc - (optind + 1);
    //argIn = optind;
    if (!help)
    {
        if (filename)
        {
            //file exist: https://stackoverflow.com/a/230068 
            if (access(out_string, F_OK))
                OUT = open(out_string, O_WRONLY | O_TRUNC);
            else
                OUT = open(out_string, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH |O_CREAT | O_WRONLY);
            if (OUT == -1) EXIT_DESTROY(WRITE_FAIL, "COULDN'T OPEN FILE");
        } else {
        }

        if (extract)
        {
            
        }

        if (create)
        {
            writeError = write(OUT, ARVIK_TAG, ARVIK_TAG_LEN);
            if (writeError == -1) EXIT_DESTROY(CREATE_FAIL, "failed to write the tag?");
            if (argCount > 0)
            {
                for (int argIn = optind; argIn < argCount; ++argIn)
                {
                    currentARG = argv[argIn];
                    setHead(currentARG);
                    setFoot(currentARG);
                    if (verbose)
                        printf("a - %s\n", currentARG);
                    IN = open(currentARG, O_RDONLY);
                    if (IN == -1) EXIT_DESTROY(OPEN_FAIL, "failed to open file");
                    writeError = write(OUT,arvikLINES->header, ARVIK_HEADER_LEN);
                    read_write();
                    writeError = write(OUT, arvikLINES->footer, ARVIK_FOOTER_LEN);
                    close(IN);
                }
            } else {
                read_write();
            }
        }
        GATEPASS(2);

        GATEPASS(3);
        if (toc)
        {
            if (verify)
            {
                if (verbose) //**VERIFY && VERBOSE (v AND V)*/
                {
                    GATEPASS(601);
                }
                else  //**VERIFY && !VERBOSE (v AND NOT V)*/
                {
                    GATEPASS(501);
                }
            } else if (verbose) {//** !VERIFY && VERBOSE (NOT v AND V)*/
                GATEPASS(401);

            } else{ //** !VERIFY && !VERBOSE (NOT v AND NOT V)*/
                GATEPASS(301);

            }
        }
    } else {
        GATEPASS(801);
        displayHelp();
    }
    GATEPASS(999);
    exit(EXIT_SUCCESS);
}
