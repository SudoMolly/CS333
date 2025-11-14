//**-----------------------------------------INCLUDES ------------------------------------*/
#include <md4.h>

#include "./arvik.h"

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
#include <openssl/err.h>

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



# pragma GCC diagnostic ignored  "-Wdeprecated"
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"


static void* holder = NULL;
//static int* fileV_INT = NULL;
static char* currentARG = NULL;
static char* intStrBUFF = NULL;
static char* inBUFF = NULL;
static char* outBUFF = NULL;
//static char** fileV_STR = NULL;
static struct stat* statHold = NULL;
//static toc_verbose_t* verboseDATA = NULL;
//static toc_verify_t* verifyDATA = NULL;
//static arvik_lines_t* arvikLINES = NULL;

//static MD4_CTX* MD4_context = NULL;
static uint8_t MD4_hash[MD4_DIGEST_LENGTH * 2];
//static unsigned int* MD4_hash_len = NULL;
static MD4_CTX* ctx = NULL;
//EVP_MD_CTX* EVP_context = NULL;
static char* EXTRACT_header = NULL;
static char* EXTRACT_footer = NULL;
static char* currentFileName = NULL;
static int EXTRACT_MAX_READ = 0;
static int EXTRACT_current_read = 0;
static int EXTRACT_total_read = 0;
static char EXTRACT_GUESS_HEADER_HASH[MD4_BLOCK_LENGTH];
//static char* EVP_message = NULL;
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

struct create_s{
    char header[79];
    char footer[67];
} create_d = {
    "                              DDDDDDDDD     UUUUU GGGGG MMMMMM  SS          $\n",
    "                                                                 \n"
};

static int CREATE_SIZE = sizeof(create_d);

typedef struct create_s create_t;


static create_t * create_t_list = NULL;
int* read_file_fail_i = NULL;


bool create_arvik(char* file)
{
    int name;
    if (IN == STDIN_FILENO) return false; //should not happen
    read_file_fail_i = (int*) calloc(fileC,sizeof(int));
    create_t_list = (create_t*) calloc(fileC, CREATE_SIZE);

    for (int i = 0 ; i < fileC; ++i)
    {
        if ((open(file, O_RDONLY)) == -1) continue;
        create_t_list[i] = create_d;
        create_t * alias = create_t_list + i;
        name = strlen(file);
        if (name > ARVIK_NAME_LEN) memcpy(create_t_list[i].header, file, ARVIK_NAME_LEN);
        else{                      memcpy(create_t_list[i].header, file, name); 
                                   alias->header[name] = ARVIK_NAME_TERM;
        }
        

        
    }
}

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
                if (access(out_string, F_OK))
                {
                    OUT = open(out_string, O_WRONLY | O_TRUNC);
                } else {
                    OUT = open(out_string, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH |O_CREAT | O_WRONLY);
                }
                if (OUT == -1) EXIT_DESTROY(WRITE_FAIL, "COULDN'T OPEN FILE");
                IN_TOC = OUT;
            } else //extract
            { 
                if (!access(out_string, F_OK))
                    EXIT_DESTROY(READ_FAIL, "ARCHIVE COULD NOT BE FOUND");
                IN = open(out_string, O_RDONLY);
                IN_TOC = open(out_string, O_RDONLY);
            }
        } else {
            GATEPASS(710);
            IN = STDIN_FILENO;
            IN_TOC = STDIN_FILENO;
        }
        GATEPASS(799);

        if (extract)
        {
            GATEPASS(200);
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
                //**EVP */
                //if (EVP_DigestInit_ex(EVP_context, EVP_md4(), NULL) != 1)
                    //EXIT_DESTROY(MD4_ERROR, "Something bad happened when initializing, idk!");

                //GET OUT THAT DATA!!!
                for(EXTRACT_total_read = 0; EXTRACT_total_read < EXTRACT_MAX_READ; EXTRACT_total_read += EXTRACT_current_read)
                {
                    EXTRACT_current_read = read(IN,inBUFF,RDWR_BUFF_SIZE);
                    if (EXTRACT_current_read == -1) EXIT_DESTROY(EXTRACT_FAIL, "FAILED TO READ IN EXTRACT");
                    if (EXTRACT_current_read == 0) EXIT_DESTROY(EXTRACT_FAIL, "REACHED EOF EARLY");
                    writeError = write(OUT,inBUFF,EXTRACT_current_read);
                    //**EVP */
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
            //DECIPHER DATA

            GATEPASS(299);
        }
        GATEPASS(2);

        if (create)
        {

            writeError = write(OUT, ARVIK_TAG, ARVIK_TAG_LEN);
            if (writeError == -1) EXIT_DESTROY(CREATE_FAIL, "failed to write the tag?");
            if (fileC > 0)
            {
                for (argIn = optind; argIn < argc ; ++argIn)
                {
                    SHOWI("\nIN CREATE FOR: ", argIn);
                    currentARG = argv[argIn];

                    GATEPASS(302);
                    setHead(currentARG);
                    GATEPASS(303);
                    startFoot();
                    GATEPASS(304);

                    if (verbose)
                        printf("a - %s\n", currentARG);

                    IN = open(currentARG, O_RDONLY);
                    if (IN == -1) EXIT_DESTROY(OPEN_FAIL, "failed to open file");
                    writeError = write(OUT,arvikLINES->header, ARVIK_HEADER_LEN);
                    if (writeError == -1) EXIT_DESTROY(WRITE_FAIL, "Failed to write header");
                    read_write();
                    endFoot();
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
                //if (holder != NULL) statHold = (struct stat*) holder;
            }
            close(OUT);
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