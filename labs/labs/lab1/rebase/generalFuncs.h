#ifndef generalFuncs_h
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#include <pthread.h>

//**MACRO DEFINE */
//#define TEST
#define DEBUG
#define CREATE_THREAD_LIST(num) (pthread_t*) malloc(sizeof(pthread_t) * num);
#define CREATE_FORK_LIST(num) (pid_t* ) malloc(sizeof(pid_t) * num);


typedef enum { Encrypt, Decrypt } encryptOrDecrypt;
/*
//Only part of code that I couldn't really figure out, union tag, gotten from:
//https://stackoverflow.com/questions/4194506/how-to-know-that-which-variable-from-union-is-used**
//Specifically answer by Tom Anderson**
typedef enum { Size, Buffer} intStringUTag;
typedef struct {
    intStringUTag UTAG;
    union {
        int size;
        char* buffer;
    } Int_String_U_Data; 
} intStringUStruct;
*/


typedef struct {
  bool e;
  bool d;
  bool c;
  char *c_opt;
  bool x;
  char *x_opt;
  bool h;
  bool D;
  //char *input;
  encryptOrDecrypt expected;
  char* buff;
  char* OUT;
} OPTIONS;

typedef struct {
  char* k;
  char* p;
  bool _cae;
  bool _xor;
  int tid;
  int keyI;
}CAEXORDATA;


#ifdef TEST
#define testp "abcdef"
#define testk "!#!#"
#define OUTCOME(type, before, after)                                           \
  printf("Before %s: %s\nAfter: %s\n\n", type, before, after)
#endif
#ifndef TEST
#define testp NULL
#define testk NULL
#endif

#ifdef DEBUG
#define SHOW(name)                                                             \
  fprintf(stderr, "Identifier: %s\nLine: %d, Function: %s, File: %s\n---\n",   \
          name, __LINE__, __func__, __FILE__)
#define VARSHOW(name, var) fprintf(stderr, "VARIABLE %s: %s \n", name, var)
// Too many typos, just do it again
#define SHOWVAR(name, var) fprintf(stderr, "VARIABLE %s: %s \n", name, var)
#endif
#ifndef DEBUG
#define SHOW(name)
#define VARSHOW(name, var)
#define SHOWVAR(name, var)
#endif

// CAEXOR ACTIONS
char xorFromK(char p, char k);
char shiftPfromK(char p, char k, bool dir);
int keyToShift(char k);
void checkProperKey(char k);
char *strToCharStr(char k, char *prev);
char *charToCharStr(char k);
int intToStrSize(int num);
char *intToStr(int num, char *buffer);

// CAEXOR MAINS
char *encrypt(char *p, char *k, bool Cae, bool Xor);
char *decrypt(char *p, char *k, bool Cae, bool Xor);
// char* xoR(char* path, char* key, bool encrypt_decrypt);
// char* cae(char* path, char* key, bool encrypt_decrypt);

// INTERPRET COMMAND LINE
void printBUFFER(char* p);
void exitFail(const char *msg, int numPointers, ...);
char *strcatdup(char *begin, char *append, bool space);
OPTIONS *initializeOPTIONS(void);
void displayOPTIONS(OPTIONS *src);
bool TESTCAEXOR(void);
void debugERRMSG(const char *msg);
void debugMSG(const char *msg);
void debugVAR(int numArgs, ...);
OPTIONS *get_options(int argc, char **argv);
void option_h(void);
char *option_e(char *cInput, char *xInput, char *path);
char *option_d(char *cInput, char *xInput, char *path);
void* deallocateOPTIONS(OPTIONS* data);
void* freeAll(int numPointers, ...);
char* strredup(char* buffer, const char* msg);
char* getInput(char* buffer);
OPTIONS* bigSplitsEn(CAEXORDATA* list, pthread_t* threads, OPTIONS* options);
void* bigEncrypt(void* data);
void* bigDecrypt(void* data);
void deduceSplits(void);
void printDATA(CAEXORDATA* data);

#endif