/*
typedef enum _Bool_
{true = 1, false = 0} bool;
#include <string.h>
#include <stdlib.h>
#include "generalFuncs.c"
*/
#include "generalFuncs.h"
#include <pthread.h>
extern bool globalDEBUG;
extern bool BIG_BOY;
extern int FULLSIZE;
extern int FULLSIZESPLITS;
extern const int MAXSPLIT;
bool* THREADS;
static int buffCount;
const int LANGLEN = 94;
const int FIRSTLANG = 32;
const int LASTLANG = 126;



void printBUFFER(char* p)
{
    int hold;
    int loopSize;

    if (p == NULL) 
    {
        fprintf(stderr, "\n\n\n\nEXIT\n\n\n\n");
        perror("EMPTY");
        exit(EXIT_FAILURE);
    }
    if ((hold = (FULLSIZE - buffCount)) < MAXSPLIT)
        loopSize = hold;
    else loopSize = MAXSPLIT;
    fprintf(stderr, "\n\n\n***\nSIZE OF FULLSIZE: %d \tSIZE OF BUFFCOUNT: %d \tloopSize: %d\t List Num: %d\n***\n\n\n", FULLSIZE, buffCount, loopSize, buffCount);
    ++buffCount;
    /*
    for (int i = 0; i < loopSize; ++i)
    {
        ++buffCount;
        if ((p[i]) == '\0')
            isNull = true;
        else if (!isprint(curr = p[i]))
            isOther = true;
        if (isOther)
            fprintf(stderr, "curr char: NON_PRINT <%d>", curr);
        else if (isNull)
            fprintf(stderr, "curr char: <NULL>");
        else
            fprintf(stderr, "curr char <%c>\n", curr);
        isOther = isNull = false;
    }
    */

}

void printDATA(CAEXORDATA* data)
{
    if (data == NULL) exit(EXIT_FAILURE);
    fprintf(stderr, "Data:\tkey: %s\tpath(10) %.10s\ttid: %d\n", data->k, data->p, data->tid);
}

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

int keyToShift(char k)
{
    int hold;
    int ret;
    char* message;
    message = NULL;
    //checkProperKey(k);
    hold = (int) k;
    ret = hold - 32;
    SHOWVAR("ADJUSTING SPACE FROM", message = intToStr(hold, message));
    SHOWVAR("ADJUSTING SPACE TO", message = intToStr(ret, message));
    if (message != NULL) free(message);
    message = NULL;
    return ret;
}

char shiftPfromK(int p, int k)
{
    p -= FIRSTLANG;
    k -= FIRSTLANG;

    return ((p + k) % (LANGLEN + 1) + FIRSTLANG);
}

char xorFromK(char p, char k)
{
    return p ^ k;
}

void* bigDecrypt(void* data)
{
    return data;
}

void* bigEncrypt(void* data)
{
    int pathL;
    int keyL;
    int keyI;
    char binarySHOW[80];
    char before;
    char after;
    char currKey;
    CAEXORDATA* myData = (CAEXORDATA*) data;

    

    #ifdef DEBUG
        //SHOW("BIG ENCRYPT!!!");
        //fprintf(stderr, "\n====ENTERING THREAD %d, with id <%lu>===\n", myData->tid, pthread_self());
        //fprintf(stderr, "-Data Passed-\nkey: >%s< \t path(5): >%.5s<\tCae: %s Xor %s \tKeyI: >%d<\n-Data End-\n\n", myData->k, myData->p, (myData->_cae ? "Yes" : "No"), (myData->_xor ? "yes" : "no"), myData->keyI);
    #endif

    pathL = strlen(myData->p);
    keyL = strlen(myData->k);
    keyI = myData->keyI;
    before = '_';
    after = '_';
    currKey = 0;
    
    #ifdef DEBUG
        //fprintf(stderr, "Path Length: %d\t Key Length: %d\n", pathL, keyL);
    #endif
    
    if (myData->_cae)
    {
        for (int pathI = 0; pathI < pathL; ++pathI)
        {
            before = myData->p[pathI];
            if (before == '\n' || before == EOF)
                continue;
            currKey = myData->k[keyI];
            after = shiftPfromK(before, currKey,0);

            ++keyI;
            keyI %= keyL;
            myData->p[pathI] = after;
            #ifdef DEBUG
            if (pathI < 3)
            {
                fprintf(stderr, "%c + %c = %c\n", before,currKey, after);
                SHOW("--");
            }
            #endif
        }
        before = '_';
        after = '_';
        currKey = 0;
        keyI = myData->keyI;
    }

    if (myData->_xor)
    {
        for (int pathI = 0; pathI < pathL; ++pathI)
        {
            before = myData->p[pathI];
            if (before == '\n' || before == EOF)
                continue;
            currKey = myData->k[keyI];
            after = xorFromK(before, currKey);

            #ifdef DEBUG
                sprintf(binarySHOW, "\n%.8B (%c)\n%.8B (%c)\n||||||||\nVVVVVVVV\n%.8B (%d)\n", before,before, currKey, currKey, after, after);
                VARSHOW("ENCODED", binarySHOW);
                memset(binarySHOW,0,80);
            #endif

            ++keyI;
            keyI %= keyL;
            myData->p[pathI] = after;
        }
    }

    #ifdef DEBUG
        //SHOW("BIG EXIT!!");
        //fprintf(stderr, "\n\n\n====EXITING THREAD %d, with id <%lu>===\n\n\n", myData->tid, pthread_self());
    #endif
    THREADS[myData->tid] = true;
    pthread_exit(&myData);
}

OPTIONS* bigSplitsEn(CAEXORDATA* list, pthread_t* threads, OPTIONS* options)
{
    int outCopySize0;
    int outCopySize1;

    buffCount = 0;
    THREADS = (bool*) calloc(FULLSIZESPLITS, sizeof(_Bool));
    for (int i = 0; i < FULLSIZESPLITS; ++i)
    {
        list[i].k = strdup(options->c_opt); 
        list[i].p = (char*) calloc(MAXSPLIT + 1, sizeof(char));
        list[i].p = strncat(list[i].p, options->buff + (i * MAXSPLIT), MAXSPLIT);
        fprintf(stderr, "loop: %d\n", i);
        printBUFFER(list[i].p);
        list[i]._cae = options->c;
        list[i]._xor = options->x;
        list[i].tid = i;
        if (i == 0) list[i].keyI = 0;
        else list[i].keyI = MAXSPLIT % strlen(options->c_opt);
        pthread_create(threads + i, NULL, bigEncrypt,(&list[i]));
        #ifdef DEBUG

            //fprintf(stderr, "loop i: %d, key: %s, tid: %d\nAssigned Text: %.10s\n", i, list[i].k, list[i].tid, list[i].p);
        #endif
    }
    #ifdef DEBUG
        fprintf(stderr, "\n===All threads assigned===\n");
    #endif
    for(int i = 0; i < (FULLSIZESPLITS / 2); ++i)
    {
        while (!THREADS[i] && !THREADS[i + 1])
        {
            fprintf(stderr, "waiting...1\n");
            sleep(1);
        }
        pthread_join(threads[i], (void*) &list[i]);
        pthread_join(threads[i], (void*) &list[i + i]);
        //SHOW("--------------------JOINED!!!--------------------------");
        fprintf(stderr, "i:%d vs. %d || of %d\n", i, FULLSIZESPLITS,(i * MAXSPLIT));
        list[i] = (CAEXORDATA) *(list + i);
        list[i] = (CAEXORDATA) *(list + i);
        printDATA(list + i);
        outCopySize0 = ((FULLSIZE - i * MAXSPLIT) > MAXSPLIT ? /*normal*/ MAXSPLIT : /*end*/ FULLSIZE - (i * MAXSPLIT));
        outCopySize1= ((FULLSIZE - i * MAXSPLIT) > MAXSPLIT ? /*normal*/ MAXSPLIT : /*end*/ FULLSIZE - (i * MAXSPLIT));
        fprintf(stderr, "loopi: %d, ", i); 
        fprintf(stderr, "loopi + 1: %d, ", i + 1); 
        fprintf(stderr, "outCopySize = %d, ", outCopySize0); 
        fprintf(stderr, "outCopySize = %d, ", outCopySize1); 
        fprintf(stderr, "strlen(list[i].p = %lu, ", strlen(list[i].p));
        fprintf(stderr, "currsize of options->out = %lu", strlen(options->OUT));
        strncat(options->OUT, list[i].p, MAXSPLIT);
        strncat(options->OUT, list[i].p, MAXSPLIT);
    }
    free(threads);
    free(list);
    list = NULL;
    threads = NULL;
    return options;
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