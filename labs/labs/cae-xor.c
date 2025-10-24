/*
typedef enum _Bool_
{true = 1, false = 0} bool;
#include <string.h>
#include <stdlib.h>
#include "generalFuncs.c"
*/
#include "generalFuncs.h"

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

    SHOW("IN ENCRYPT");
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

            printVAR = strToCharStr(before, printVAR);
            VARSHOW("BEFORE", printVAR);

            after = shiftPfromK(before, k[keyI],0);

            printVAR = strToCharStr(k[keyI], printVAR);
            VARSHOW("USING: ", printVAR);
            printVAR = intToStr(keyToShift(k[keyI]), printVAR);
            VARSHOW("SHIFTING: ", printVAR);

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

            sprintf(binarySHOW, "\n%.8B (%c)\n%.8B (%c)\n||||||||\nVVVVVVVV\n%.8B (%c)\n", before,before, k[keyI], k[keyI], after, after);
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
            sprintf(binarySHOW, "\n%B (%c)\n%B (%c)\n||||||||\nVVVVVVVV\n%B (%c)", before,before, k[keyI], k[keyI], after, after);
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
            printVAR = strToCharStr(before, printVAR);
            VARSHOW("BEFORE", printVAR);
            after = shiftPfromK(before, k[keyI],1);
            printVAR = strToCharStr(k[keyI], printVAR);
            VARSHOW("USING: ", printVAR);
            printVAR = intToStr(k[keyI], printVAR);
            VARSHOW("SHIFTING: ", printVAR);
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