/*
typedef enum _Bool_
{true = 1, false = 0} bool;
#include <string.h>
#include <stdlib.h>
#include "generalFuncs.c"
*/
//**-------------------------------|INTRODUCTION|-----------------------------*/
/*
*   Molly Diaz, modiaz@pdx.edu, CS333
*   Lab 1 (cae-xor) : redux
*
*/
//**-------------------------------|  INCLUDES  |-----------------------------*/

//#include "generalFuncs.h"
//#include "xorfuncs.c"
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
//**-------------------------------|   DEFINE   |-----------------------------*/
#define BUFFSIZE 1000

#define max(a,b) (a >= b ? a : b)
//#define TESTING
#ifdef TESTING
  #define TEST(string,where) {fprintf(stderr, "\n--%s--\n", where); for (long unsigned int i = 0; i < BUFFSIZE; ++i) {fprintf(stderr, "%d",string[i]);}}
#endif
#ifndef TESTING
  #define TEST(string, where)
#endif

//*========RD /WR===========*//
#define CHARSIZE sizeof(char)
#define E2BINARY 0xe2
#define UTFBYTECHECK(character) ((character ^ E2BINARY) == 0 ? true : false)

//*========CAEXOR===========*//

#define LANGLOW ' '
#define LANGSIZE '^'
#define keyIget(keyIndex, keyLength) keyIndex = ((keyIndex + 1) % keyLength)
#define keyEncryptShow(before, after, shift) {if (verbose) fprintf(E_OUT, "%c->%c\t(%c shifted %d characters to %c)\n", before,after,before,shift,after);}

//*========OPTIONS==========*//

#define OPTS ":edc:x:hD"
#define ENCRYPT 'e'
#define DECRYPT 'd'
#define CAE 'c'
#define XOR 'x'
#define HELP 'h'
#define DIAG 'D'
#define NO_OPTARG ':'
#define UNKNOWN_ARG '?'
#define MIN_LANG ' '
#define MAX_LANG '~'

//*========ERROR============*//

#define NULL_UNEXPECTED_DEF NULL_UNEXPECTED
#define READ_ERROR_DEF READ_ERROR
#define WRITE_ERROR_DEF WRITE_ERROR
#define XOR_FAIL_DEF XOR_FAIL
#define CAE_FAIL_DEF CAE_FAIL
#define ENCOUNTERED_EOF_DEF EOF_FOUND
#define NO_OPTARG_DEF MY_NO_OPTARG
#define UNKNOWN_OPTARG_DEF UNKNOWN_OPTARG
#define UNKNOWN_GETOPT_ERROR_DEF UNKNOWN_GETOPT_ERROR
#define ERROR_MAX_DEF ERROR_MAX


#define _FOR_EACH_ERROR(ERROR_LIST)                                                  \
  ERROR_LIST(NULL_UNEXPECTED_DEF)                                                       \
  ERROR_LIST(READ_ERROR_DEF)                                                            \
  ERROR_LIST(WRITE_ERROR_DEF)                                                           \
  ERROR_LIST(XOR_FAIL_DEF)                                                              \
  ERROR_LIST(CAE_FAIL_DEF)                                                              \
  ERROR_LIST(ENCOUNTERED_EOF_DEF) \
  ERROR_LIST(NO_OPTARG_DEF)       \
  ERROR_LIST(UNKNOWN_OPTARG_DEF)  \
  ERROR_LIST(UNKNOWN_GETOPT_ERROR_DEF)   \
  ERROR_LIST(ERROR_MAX_DEF)


#define _GENERATE_ENUM(ENUM) ENUM,
#define _GENERATE_STRING(STRING) #STRING,


//**--------------------------------| STRUCTURE |-----------------------------*/

typedef enum error_list_e {
  _FOR_EACH_ERROR(_GENERATE_ENUM)
} _error_list;

typedef struct options_s {
  struct toggles_s {
    union encrypt_u {
      bool encrypt;
    } encrypt;
    struct booleans {
      bool cae;
      bool xor;
      bool help;
      bool diag;
    } booleans;
  } toggles;
  struct strings_s {
    char *cae;
    char *xor;
  } strings;
  int last;
} options;



//**--------------------------------|  GLOBALS  |----------------------------*/
static int IN = STDIN_FILENO;
static int OUT = STDOUT_FILENO;
static int E_OUT = STDERR_FILENO;
static bool globalDEBUG = false;
options def_temp = {.toggles.encrypt.encrypt = true,
                    .toggles.booleans.cae = false,
                    .toggles.booleans.xor = false,
                    .toggles.booleans.help = false,
                    .toggles.booleans.diag = false,
                    .strings.cae = NULL,
                    .strings.xor = NULL,
                    .last = -1};

[[maybe_unused]] static const char* ERROR_NAMES[] = {
  _FOR_EACH_ERROR(_GENERATE_STRING)
};

//**--------------------------------|PROTOTYPES |-----------------------------*/
int strsize(int num);
char* error_msg(int line, const char *file, const char *func, int errorno);
void gatepass_func(const char * msg, int line, const char* func, const char* file);
char shiftleft(int path, int key);
char shiftright(int p, int k);
uint8_t xor_alg(uint8_t path,uint8_t key);
const char* xor_col(bool a, bool b);
const char* xor_one_col(bool a);
char* xor(char* path, char* key, int length);
char* cae(char* path, char* key, bool encrypt);
char* encrypt(char* path, char* keyC, char* keyX);
char* decrypt(char* path, char* keyC, char* keyX, int num_read);
int file_size(int fd);
int read_buff(char *buffer, int fd, int bite);
bool write_buff( char *buffer, int fd,int bite, int num_read);
bool getoptions(int argc, char **argv, options *opts);
void help_exit(void);
//**--------------------------------| FUNCTIONS |-----------------------------*/

//*========ERROR============*//
#define RESET "\e[0m"
int strsize(int num) {
  int hold = 10;
  int i = 1;
  for (i = 1; num > 0; ++i) {
    if ( (num /= (hold *= 10) ) == 0)   break;
  }
  return i;
}

char* error_msg(int line, const char *file, const char *func, int errorno) {
  static char * msg_line;
  const char* error_name = ERROR_NAMES[errorno];
  int size = (strlen(file)) + (strlen(func)) + (strlen(error_name)) + strsize(line);
  msg_line = malloc(CHARSIZE * (size + 1));
  msg_line[size] = '\0';
  snprintf(msg_line, size + 1, ">[%s] occurred on line: #%d\t in function: [%s]\t in file: [%s]<", error_name, line, func, file);
  return msg_line;
}

void gatepass_func(const char * msg, int line, const char* func, const char* file)
{
  if (!globalDEBUG) return;
  dprintf(E_OUT, "L: %d\tFu: %s\tFi: %s\n", line, func, file);
  dprintf(E_OUT, "%s\n", msg);
}

#define GATEPASS(msg) gatepass_func(msg, __LINE__, __func__, __FILE__)

#define ERROR_MSG(ERROR)  perror(error_msg(__LINE__, __FILE__, __func__, ERROR))
  
#define ERROR_CHECK(ifd,errorno,do_this) if (ifd) {ERROR_MSG(errorno); do_this;}
  
//*========CAEXOR============*//

char shiftleft(int path, int key) {
  int hold;
  if (key == ' ') return path;
  key -= LANGLOW;
  hold = (path - key);
  if (hold < 32) hold+=95;
  return hold;
}

char shiftright(int p, int k) {
  //if (k == ' ') return p;
  char hold;
  if (k == ' ') return p;
  p -= LANGLOW;
  k -= LANGLOW;
  //hold = (p + k) % (127);
  hold = (p + k) % 95;
  hold+=32;
  //if (hold < 32) hold += 32;
  return hold;
}

uint8_t xor_alg(uint8_t path, uint8_t key) { return path ^ key; }

const char* xor_col(bool a, bool b)
{
  if ((a ^ b)) return "\e[0;32m";
  return "\e[0;31m";
}

const char* xor_one_col(bool a)
{
  if (a) return "\e[43m";
  return "";
}

#define COLOR_RED "\e[1;31m"
#define COLOR_GREEN "\e[1;32m"

char* xor(char* path, char* key, int length)
{
    int pathI,pathL, keyI, keyL;
    char before, using, after;

    char empty[] = {0};
//    char beforeB[] = "00000000";
    //char usingB[]  = "00000000";
    //char afterB[]  = "00000000";

    TEST(path, "XOR");
    pathL = length;
    if (key == NULL) {key = empty; keyL = 1;}
    else keyL  = strlen(key);
    keyI = 0;
    for (pathI = 0; pathI < pathL; ++pathI)
    {
        before = path[pathI];
        //ERROR_CHECK((before == EOF), EOF_FOUND, return NULL);
        using = key[keyI];
        after = xor_alg(before,using);
        path[pathI] = after;
        /*
        if (globalDEBUG)
        {
            snprintf(beforeB, 9, "%.8B", before);
            snprintf(usingB,9, "%.8B", using);
            snprintf(afterB, 9, "%.8B", after);
            dprintf(E_OUT, "from (%c) to (%c)\n", before, after);
            dprintf(E_OUT, "before: %s\n        ||||||||\nusing : %s\n        ||||||||\nafter : ", beforeB, usingB);
            for (int i = 0; i < 9; ++i)
            {
                bef = beforeB[i];
                usi = usingB[i];
                aft = afterB[i];
                if ((bef ^ usi) == 0)
                  dprintf( E_OUT,"%s%c%s",COLOR_RED, bef, RESET);
                else
                  dprintf( E_OUT,"%s%c%s",COLOR_GREEN, aft, RESET);
            }
            dprintf(E_OUT, "\n");
        }
        */
        ++keyI;
        keyI %= keyL;
        if (globalDEBUG)
          fprintf(stderr, "From (0x%.2X) using (0x%.2X) to (0x%.2X): (0x%X -> 0x%.2X)\n", before,using,after,before,after);
    }
  TEST(path, "END OF XOR");
    return path;
}

char* cae(char* path, char* key, [[maybe_unused]] bool encrypt)
{
  //make final
  int pathI, pathL, keyL;
  int before;
  int using;
  char after;
  static int keyI = 0;
  
  TEST(path, "CAE");
  pathL = strlen(path);
  keyL = strlen(key);
  for (pathI = 0; pathI < pathL; ++pathI)
  {
      before = path[pathI];
      //if (before < 0) dprintf(E_OUT, "\nBEFORE IS NOT (%c) THE SAME AS PATH (%c)", before, path[pathI]);
      if (globalDEBUG && path[pathI] < 0)
        fprintf(stderr, "NUMB NEGATIVE? : (%d)", path[pathI]);
 //     if (UTFBYTECHECK(before))
      //{
        //if (globalDEBUG) 
          //fprintf(stderr, "UTF FOUND IN POS(%d - %d) VALUES: (%d,%d,%d)", pathI, pathI + 2, pathI, pathI + 1, pathI + 2);
        //pathI += 2;
        //continue;
      //}
      if (before < 32)
          continue;
      //ERROR_CHECK((before == EOF), EOF_FOUND, return NULL);
      using = key[keyI];
      if (encrypt) {after = shiftright(before,using);}
      else         {after = shiftleft(before,using);}
      path[pathI] = after;
      if (globalDEBUG)
          dprintf(E_OUT, "%c -> %c, moving from %c(%d) using %c(%d) results %c(%d)\n", before,after,before, before - 32, using, using - 32, after, after - 32);
      ++keyI;
      keyI %= keyL;
  }
  return path;
}

char* encrypt(char* path, char* keyC, char* keyX)
{
  TEST(path, "encrypt");
  if (keyC == NULL)
  {
    if (globalDEBUG)
      fprintf(stderr, "\n****SKIPPING CAE, EMPTY****\n");
  } else {
    if (globalDEBUG)
      dprintf(E_OUT, "\nbefore = %s\n", path);
    path = cae(path, keyC, true);
    ERROR_CHECK((path == NULL), CAE_FAIL, if (globalDEBUG) dprintf(E_OUT, "Cae failed, exitting..."); return NULL);
    if (globalDEBUG)
        dprintf(E_OUT, "using = %s\nafter = %s\n", keyC, path);
  }
  if (keyX == NULL)
  {
    if (globalDEBUG)
      fprintf(stderr, "\n****SKIPPING XOR, EMPTY****\n");
  } else {
    if (globalDEBUG)
      dprintf(E_OUT, "\nbefore = %s\n", path);
    path = xor(path, keyX, strlen(path));
    ERROR_CHECK((path == NULL), CAE_FAIL, if (globalDEBUG) dprintf(E_OUT, "Xor failed, exitting..."); return NULL);
    if (globalDEBUG)
        dprintf(E_OUT, "using = (%s)\nafter = (%s)\n", keyX, path);
  }

  return path;
}
char* decrypt(char* path, char* keyC, char* keyX, int num_read)
{
  TEST(path, "decrypt");
  if (keyX == NULL)
  {
    if (globalDEBUG)
      fprintf(stderr, "\n****SKIPPING XOR, EMPTY****\n");
  } else {
    if (globalDEBUG)
      dprintf(E_OUT, "\nbefore = %s\n", path);
    path = xor(path, keyX, num_read);
    ERROR_CHECK((path == NULL), CAE_FAIL, if (globalDEBUG) dprintf(E_OUT, "Xor failed, exitting..."); return NULL);
    if (globalDEBUG)
        dprintf(E_OUT, "using = %s\nafter = %s\n", keyX, path);
  }

  if (keyC == NULL)
  {
    if (globalDEBUG)
      fprintf(stderr, "\n****SKIPPING CAE, EMPTY****\n");
  } else{
    if (globalDEBUG)
      dprintf(E_OUT, "\nbefore = %s\n", path);
    path = cae(path, keyC, false);
    ERROR_CHECK((path == NULL), CAE_FAIL, if (globalDEBUG) dprintf(E_OUT, "Cae failed, exitting..."); return NULL);
    if (globalDEBUG)
        dprintf(E_OUT, "using = %s\nafter = %s\n", keyC, path);
  }

  return path;
}

//*========RD / WR============*//
int file_size(int fd) {
  int bottom;
  bottom = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  return bottom;
}

//ASSUMES BUFFER == size, if not error & realloc size
int read_buff(char *buffer, int fd, int bite) {
  int readed;
  ERROR_CHECK((buffer == NULL),NULL_UNEXPECTED, return -1);
  memset(buffer, 0, BUFFSIZE + 1);
  readed = read(fd, buffer, CHARSIZE * bite);
  ERROR_CHECK((readed == -1),READ_ERROR, return -1);
  if (globalDEBUG)
    fprintf(stderr, "\n\nREAD: %s\n\n",buffer);
  //if (readed == -1) ERROR_MSG(READ_ERROR); return NULL;
  TEST(buffer, "read");
  if (globalDEBUG)
    fprintf(stderr, "READ: %d chars\n", readed);
  return readed;
}

bool write_buff(char *buffer, int fd,int bite, int num_read) {
  int hold = 0;
  if (globalDEBUG)
  {
    fprintf(stderr, "VALUES GOING INTO WRITE BUFFER:\n buffer: %s\nfile descriptor: %d\nbite: %d\n", buffer,fd,bite);
    TEST(buffer,"WRITING")
    fprintf(stderr, "\n");
  }

  if (bite == 0){
    fprintf(stderr, "bite is empty\n");
    return false;
  } else {
    if (globalDEBUG)
      fprintf(stderr, "PRINTING %s\n", buffer);
    do
    {
      hold = write(fd, buffer + hold, bite);
      if (globalDEBUG) fprintf(stderr, "VALUE OF HOLD : %d && errno: %d\n", hold, errno);
      if (hold == -1)  return false;
    } while (hold < num_read);
  }
  TEST(buffer, "write");
  return true;
}

//*========OPTIONS============*//
bool getoptions(int argc, char **argv, options *opts) {
  char opt;
  bool help = true;
  while ((opt = (getopt(argc, argv, OPTS))) != -1) {
    help = false;
    switch (opt) {
    case ENCRYPT:
      opts->toggles.encrypt.encrypt = true;
      break;
    case DECRYPT:
      opts->toggles.encrypt.encrypt = false;
      break;
    case CAE:
      opts->toggles.booleans.cae = true;
      opts->strings.cae = strdup(optarg);
      ERROR_CHECK((opts->strings.cae == NULL), CAE_FAIL_DEF, return false;)
      break;
    case XOR:
      opts->toggles.booleans.xor = true;
      opts->strings.xor = strdup(optarg);
      ERROR_CHECK((opts->strings.xor == NULL), XOR_FAIL_DEF, return false;)
      break;
    case HELP:
      opts->toggles.booleans.help = help = true;
      break;
    case DIAG:
      opts->toggles.booleans.diag = globalDEBUG = true;
      break;
    case NO_OPTARG:
      ERROR_MSG(NO_OPTARG_DEF);
      return false;
      break;
    case UNKNOWN_ARG:
      ERROR_MSG(UNKNOWN_OPTARG_DEF);
      return false;
      break;
    default:
      ERROR_MSG(UNKNOWN_GETOPT_ERROR_DEF);
      return false;
      break;
    }
  }
  opts->last = optind;
  return help;
}

void help_exit(void){ printf("help\n");}

int main(int argc, char **argv) {
  int exit_value, read_ret;
  char *returned;
  bool help, error;
  options opts;
  char BUFFER[BUFFSIZE + 1];
  error = false;
  opts = def_temp;
  exit_value = 0;
  returned = NULL;
  help = getoptions(argc,argv,&opts);
  if (!help)
  {
    if (opts.toggles.encrypt.encrypt) { //Encrypt
      do
      {
        read_ret = read_buff(BUFFER, IN, BUFFSIZE);
        if (read_ret == -1) {error = true; perror("Failed to read, encrypt"); break;}
        if (read_ret == 0) break;
        else{
          if (globalDEBUG) fprintf(stderr, "READ RET VALUE : %d\n", read_ret);
          returned = encrypt(BUFFER,opts.strings.cae,opts.strings.xor);
          if(!write_buff(returned, OUT, read_ret,read_ret)) {error = true; perror("Failed to write, encrypt"); break;}
          returned = NULL;
        }
      } while(read_ret != 0);
    } else{ // Decrypt
      do
      {
        read_ret = read_buff(BUFFER, IN, BUFFSIZE);
        if (read_ret == -1) {error = true; perror("Failed to read, decrypt");break;}
        if (read_ret == 0) break;
        else{
          returned = decrypt(BUFFER,opts.strings.cae,opts.strings.xor, read_ret);
          if(!write_buff(returned, OUT, read_ret, read_ret)) {error = true; perror("Failed to write, decrypt"); break;}
          returned = NULL;
        }
      } while(read_ret != 0);
    }
    //if (!write_buff("\n", OUT, 1)) error =true;
  }
  else help_exit();
  if (error)
  {
    GATEPASS("FAILED HAPPENED");
    fprintf(stderr, "ERROR NUMBER: %d\n", errno);
    exit_value = EXIT_FAILURE;
  }
  if (opts.strings.cae != NULL)
  {
    free(opts.strings.cae);
    opts.strings.cae = NULL;
  }
  if (opts.strings.xor != NULL)
  {
    free(opts.strings.xor);
    opts.strings.xor = NULL;
  }
  return exit_value;
}

/*
char* encrypt(char* p, char* k, bool Cae, bool Xor)
{
    int pathL;
    int keyL;
    int keyI;
    char* hold;
    char* ret;
    char* printVAR;
    char binarySHOW[80];
    char before;
    char after;
    char* TRUE;
    char* FALSE;
    char* cae_p;
    char* xor_p;
    hold = NULL;
    ret = NULL;
    printVAR = NULL;
    cae_p = NULL;
    xor_p = NULL;

    TRUE = strdup("true");
    FALSE = strdup("false");

    SHOW("IN ENCRYPT");
    if (globalDEBUG)
    {
        debugMSG("Encrypt Vars");
        cae_p = (Cae ? TRUE : FALSE);
        xor_p = (Xor ? TRUE : FALSE);
        debugVAR(4, "Path", p, "Key", k, "Is Cae?", cae_p, "Is Xor?", xor_p);
    }
    if (p == NULL || k == NULL)
    {
        perror("No path or key passed");
        exit(EXIT_FAILURE);
    }

    pathL = strlen(p);
    keyL = strlen(k);
    keyI = 0;
    hold = strdup(p);
    ret = strdup(p);
    printVAR = charToCharStr('_');

    before = '_';
    after = '_';
    
    SHOW("SETTING PATH TO RET");
    VARSHOW("PATH", p);
    VARSHOW("RET", ret);

    if (Cae)
    {
        SHOW("INTO CAE");
        for (int pathI = 0; pathI < pathL; ++pathI)
        {
            before = hold[pathI];
            if (before == '\n' || before == EOF)
                continue;
            printVAR = strToCharStr(before, printVAR);
            VARSHOW("BEFORE", printVAR);

            after = shiftPfromK(before, k[keyI],0);
            printVAR = strToCharStr(after, printVAR);
            VARSHOW("AFTER", printVAR);

            printVAR = strToCharStr(k[keyI], printVAR);
            VARSHOW("USING", printVAR);
            printVAR = intToStr(keyToShift(k[keyI]), printVAR);
            VARSHOW("SHIFTING RIGHT", printVAR);

            ++keyI;
            keyI %= keyL;
            ret[pathI] = after;

            printVAR = strToCharStr(after, printVAR);
            VARSHOW("AFTER", printVAR);
        }
        before = '_';
        after = '_';
        keyI = 0;
        hold = strcpy(hold, ret);
        SHOW("OUT CAE");
        VARSHOW("PATH", p);
        VARSHOW("KEY", k);
        VARSHOW("HOLDING", hold);
        VARSHOW("RETURNING", ret);
    }

    if (Xor)
    {
        SHOW("INTO XOR");
        for (int pathI = 0; pathI < pathL; ++pathI)
        {
            before = hold[pathI]; //&p[i]
            printVAR = strToCharStr(before, printVAR);
            after = xorFromK(before, k[keyI]);

            sprintf(binarySHOW, "\n%.8B (%c)\n%.8B (%c)\n||||||||\nVVVVVVVV\n%.8B (%d)\n", before,before, k[keyI], k[keyI], after, after);
            VARSHOW("ENCODED", binarySHOW);
            memset(binarySHOW,0,80);

            ++keyI;
            keyI %= keyL;
            ret[pathI] = after;
            printVAR = strToCharStr(after, printVAR);
        }
        SHOW("OUT XOR");
        VARSHOW("PATH", p);
        VARSHOW("KEY", k);
        VARSHOW("HOLDING", hold);
        VARSHOW("RETURNING", ret);
    }

    SHOW("OUT ENCRYPT");
    free(hold);
    free(printVAR);
    free(TRUE);
    free(FALSE);
    hold = printVAR = TRUE = FALSE = NULL;
    return ret;
}

char* decrypt(char* p, char* k, bool Cae, bool Xor)
{
    int pathL;
    int keyL;
    int keyI;
    char* hold;
    char* ret;
    char* printVAR;
    char binarySHOW[80];
    char before;
    char after;

    SHOW("IN DECRYPT");
    if (p == NULL || k == NULL)
    {
        perror("No path or key passed");
        exit(EXIT_FAILURE);
    }

    pathL = strlen(p);
    keyL = strlen(k);
    keyI = 0;
    hold = strdup(p);
    ret = strdup(p);
    printVAR = charToCharStr('_');
    
    before = '_';
    after = '_';
    
    SHOW("SETTING PATH TO RET");
    VARSHOW("PATH", p);
    VARSHOW("RET", ret);

    if (Xor)
    {
        SHOW("INTO XOR");
        for (int pathI = 0; pathI < pathL; ++pathI)
        {
            before = hold[pathI]; //&p[i]
            printVAR = strToCharStr(before, printVAR);
            VARSHOW("BEFORE", printVAR);
            after = xorFromK(before, k[keyI]);
            VARSHOW("DECODED", binarySHOW);
            sprintf(binarySHOW, "\n%.8B (%c)\n%.8B (%c)\n||||||||\nVVVVVVVV\n%.8B (%d)", before,before, k[keyI], k[keyI], after, (int) after);
            memset(binarySHOW,0,80);


            ++keyI;
            keyI %= keyL;
            ret[pathI] = after;
            printVAR = strToCharStr(after, printVAR);
            VARSHOW("AFTER", printVAR);
        }
        before = '_';
        after = '_';
        keyI = 0;
        hold = strcpy(hold, ret);
        SHOW("OUT XOR");
    }

    if (Cae)
    {
        SHOW("INTO CAE");
        for (int pathI = 0; pathI < pathL; ++pathI)
        {
            before = hold[pathI];
            if (before == '\n' || before == EOF)
                continue;
            printVAR = strToCharStr(before, printVAR);
            VARSHOW("BEFORE", printVAR);
            after = shiftPfromK(before, k[keyI],1);
            printVAR = strToCharStr(k[keyI], printVAR);
            VARSHOW("USING", printVAR);
            printVAR = intToStr(k[keyI], printVAR);
            VARSHOW("SHIFTING", printVAR);
            ++keyI;
            keyI %= keyL;
            ret[pathI] = after;
            printVAR = strToCharStr(after, printVAR);
            VARSHOW("AFTER", printVAR);
        }
        SHOW("OUT CAE");
    }

    

    SHOW("OUT DECRYPT");
    free(hold);
    free(printVAR);
    return ret;
}
*/
/*
char* cae(char* path, char* key, bool encrypt_decrypt)
{    
    int keyL;
    int pathL;
    char* newPath;
    int keyI;
    if (path == NULL || key == NULL)
    {
        perror("Entered null string");
        exit(EXIT_FAILURE);
    }

    keyL = strlen(key);
    pathL = strlen(path);
    
    newPath = strdup(path);
    //decrypt
    if (encrypt_decrypt)
    {
        for (int i = 0; i < pathL; ++i)
        {
            newPath[i] = shiftChar(path[i], -findCaeShift(key[keyI]));
            keyI = (keyI + 1) % keyL;
        }
    }

    //encrypt
    else
    {
        for (int i = 0; i < pathL; ++i)
        {
            newPath[i] = shiftChar(path[i], findCaeShift(key[keyI]));
            keyI = (keyI + 1) % keyL;
        }
    }
    
    free(path);
    path = newPath;
    return newPath;
}

char* xoR(char* path, char* key, bool encrypt_decrypt)
{
    int keyL;
    int pathL;
    int keyI;
    char* newPath;
    if (path == NULL || key == NULL)
    {
        perror("Entered null string");
        exit(EXIT_FAILURE);
    }
    keyL = strlen(key);
    pathL = strlen(path);
    keyI = 0;
    newPath = strdup(path);
    //decrypt
    if (encrypt_decrypt)
    {
        for (int i = 0; i < pathL; ++i)
        {
            newPath[i] = shiftByte(path[i], key[keyI]);
            keyI = (keyI + 1) % keyL;
        }
    }
    else
    {
        for (int i = 0; i < pathL; ++i)
        {
            newPath[i] = shiftChar(path[i], -key[keyI]);
            keyI = (keyI + 1) % keyL;
        }
    }

    free(path);
    path = newPath;
    return newPath;
}
*/