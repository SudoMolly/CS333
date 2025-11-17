#include "generalFuncs.h"


char* strToCharStr(char k, char* prev)
{
    char* hold;
    if (prev == NULL)
        return charToCharStr(k);
    else
        hold = (char*) realloc(prev, 2);
    hold[0] = k;
    return hold;
}


int intToStrSize(int num)
{
    int size;
    int hold;
    size = 1;
    hold = num;
    do
    {
        hold /= 10;
        ++size;
    } while(hold != 0);
    return size;
}

char* intToStr(int num, char* buffer)
{
    int size = intToStrSize(num);
    if (buffer == NULL)
        buffer = (char*) malloc(size);
    else 
        buffer = (char*) realloc(buffer,size);
    sprintf(buffer, "%d", num);
    return buffer;
}

char* charToCharStr(char k)
{   
    char* ret = (char*) malloc(2);
    ret[0] = k;
    return ret;
}

void checkProperKey(char k)
{
    //IF POSITIVE OR NEGATIVE OUT OF BOUNDS
    //if k greater than 0 OR less than -128
    //AND IF OUT OF BOUNDS
    //AND if k is less than 32 OR greater than 126
    if ( k > 0 && (k < 32 || k > 126))
    {
        perror("Cannot use non-printable ascii key");
        exit(EXIT_FAILURE);
    }
}

int keyToShift(char k)
{
    checkProperKey(k);
    return k - 32;
}


char shiftPfromK(char p, char k, bool dir)
{
    int shift;
    char ret;
    shift = keyToShift(k);
    if (dir)
        ret = p - shift;
    else
        ret = p + shift;
    if (ret < 32)
        ret = ' ';
    else if (ret > 126)
        ret = '~';
    return ret;
}

char xorFromK(char p, char k)
{
    return p ^ k;
}



int main(int argc, char** argv)
{
    char* hold;
    char* hold2;
    char* path;
    char* key;        char* hold3;
    char* hold4;
    char* hold5;
    char* hold6;
    bool caeSuccess;
    bool xorSuccess;
    bool caexorSuccess;
    int passed;

    #ifdef TEST
        passed = 0;
        path = testp;
        key = testk;
        VARSHOW("PATH", path);
        VARSHOW("KEY", key);
        SHOW("====TEST BEGIN====");
        SHOW("--ENCRYPT BEGIN--");
        SHOW("ENCRYPT CAE");
        hold = encrypt(path,key,true,false);
        SHOW("ENCRYPT XOR");
        hold2 = encrypt(path, key, false, true);
        SHOW("ENCRYPT CAE-XOR");
        hold3 = encrypt(path, key, true ,true);

        SHOW("--OVER. CHECK--");
        VARSHOW("CAE", hold);
        VARSHOW("XOR", hold2);
        VARSHOW("CAE-XOR", hold3);

        SHOW("--DECRYPT BEGIN--");
        SHOW("DECRYPT CAE");
        hold4 = decrypt(hold, key, true, false);
        SHOW("DECRYPT XOR");
        hold5 = decrypt(hold2, key, false ,true);
        SHOW("DECRYPT CAE-XOR");
        hold6 = decrypt(hold3, key, true, true);

        SHOW("--OVER. CHECK--");
        VARSHOW("CAE", hold4);
        VARSHOW("XOR", hold5);
        VARSHOW("CAE-XOR", hold6);

        caeSuccess = !strcmp(hold4,path);
        xorSuccess = !strcmp(hold5,path);
        caexorSuccess = !strcmp(hold6, path);

        SHOW("====TEST OVER====");
        if (caeSuccess) 
        {
            fprintf(stderr, "CAE SUCCESSFUL");
            ++passed;
        }
        if (xorSuccess)
        {
            fprintf(stderr, "XOR SUCCESSFUL");
            ++passed;
        }
        if (caexorSuccess)
        {
            fprintf(stderr, "CAEXOR SUCCESSFUL");
            ++passed;
        }
        fprintf(stderr, "%d / 3 PASSED", passed);
        
    #endif
    hold = *argv;
    printf("%d", argc);
}