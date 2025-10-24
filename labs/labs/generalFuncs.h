#ifndef generalFuncs_h
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TEST 
//#define DEBUG 

#ifdef TEST
    #define testp "abcdef"
    #define testk "!#!#"
    #define OUTCOME(type, before, after) printf("Before %s: %s\nAfter: %s\n\n", type,before,after)
#else 
    #define testp
    #define testk
#endif

#ifdef DEBUG
    #define SHOW(name) fprintf(stderr, "Identifier: %s\nLine: %d, Function: %s, File: %s\n---\n", name, __LINE__, __func__, __FILE__)
    #define VARSHOW(name, var) fprintf(stderr, "VARIABLE %s: %s \n", name, var)
#else
    #define SHOW(name)
    #define VARSHOW(name, var)
#endif

//CAEXOR ACTIONS
char xorFromK(char p, char k);
char shiftPfromK(char p, char k, bool dir);
int keyToShift(char k);
void checkProperKey(char k);
char* strToCharStr(char k, char* prev);
char* charToCharStr(char k);
int intToStrSize(int num);
char* intToStr(int num, char* buffer);

//CAEXOR MAINS
char* encrypt(char* p, char* k, bool Cae, bool Xor);
char* decrypt(char* p, char* k, bool Cae, bool Xor);
//char* xoR(char* path, char* key, bool encrypt_decrypt);
//char* cae(char* path, char* key, bool encrypt_decrypt);

//INTERPRET COMMAND LINE

#endif