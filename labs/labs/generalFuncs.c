#include "generalFuncs.h"
#include <stdlib.h>
#include <string.h>

bool globalDEBUG = false;

typedef struct {
    bool e;
    bool d;
    bool c;
    char* c_opt;
    bool x;
    char* x_opt;
    bool h;
    bool D;
    char* input;
} OPTIONS;



char* strcatdup(char* begin, char* append, bool space)
{
    int bLength;
    int aLength;
    int sum;
    char* hold;
    if (begin == NULL)
    {
        perror("Beginning (param 1) string is unintialized, in strdupcat");
        exit(EXIT_FAILURE);
    }
    if (append == NULL)
    {
        perror("End (param 2) string is unintialized, in strdupcat");
        exit(EXIT_FAILURE);
    }
    bLength = strlen(begin);
    aLength = strlen(append);
    if (space) ++aLength;
    sum = aLength + bLength;
    
    hold = (char*) malloc(sum);
    hold = strcpy(hold, begin);
    if (space) 
    {
        hold[bLength - 1] = ' ';
        hold[bLength] = '\0';
    }
    begin = (char*) realloc(begin, sum);
    begin = strcpy(begin, hold);
    free(hold);
    begin = strcat(begin,append);
    return begin;
}

OPTIONS* intializeOPTIONS()
{
    OPTIONS* ptr;
    if (ptr != NULL) //assume want to reset
        ptr = (OPTIONS*) realloc(ptr, sizeof(OPTIONS));
    else
        ptr = (OPTIONS*) malloc(sizeof(OPTIONS));
    ptr->e = false;
    ptr->d = false;
    ptr->c = false;
    ptr->c = false;
    ptr->c_opt = NULL;
    ptr->x = false;
    ptr->x_opt = NULL;
    ptr->h = false;
    ptr->D = false;
    ptr->input = NULL;
    return ptr;
}

void displayOPTIONS(OPTIONS* src)
{
    
    char falseTEXT[] = " FALSE\n";
    char trueTEXT[] = " TRUE\n";
    
    if (src == NULL)
        fprintf(stderr, "NO OPTIONS PROVIDED, CANNOT DISPLAY");
    else
    {
        fprintf(stderr, "===OPTION STRUCTURE===\n");
        fprintf(stderr, "Encrypt:\t");
        if (ptr->e)
        fprintf(stderr, "Decrypt:\t");
        fprintf(stderr, "Cae-:\t");
        fprintf(stderr, "Xor:\t");
        fprintf(stderr, "Help:\t");
        fprintf(stderr, "Debug:\t");
    }

}

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

OPTIONS* get_options(int argc, char** argv)
{
    int opt;
    int optnum;
    char * hold;
    OPTIONS* ret;
    ret = intializeOPTIONS();
    while ((opt = getopt(argc,argv, ":edc:x:hD")) != -1)
    {
        switch (opt)
        {
            case 'e':
                ret->e = !ret->e;
                break;
            case 'd':
                ret->d = !ret->d;
                break;
            case 'c':
                ret->c = !ret->c;
                if (ret->c_opt != NULL)
                    ret->c_opt = strcatdup(ret->c_opt, optarg, false);
                else
                    ret->c_opt = strdup(optarg);
                break;
            case 'x':
                ret->x = !ret->x;
                if (ret->x_opt != NULL)
                    ret->x_opt = strcatdup(ret->x_opt, optarg, false);
                else
                    ret->x_opt = strdup(optarg);
                break;
            case 'h': //YAY!
                ret->h = true;
                break;
            case 'D':
                ret->D = !ret->D;
                break;
            case ':':
                fprintf(stderr, "ERROR, OPTION NOT GIVEN ARGUMENT, CONSULT HELP!!");
                free(ret);
                exit(EXIT_FAILURE);
            case '?':
                fprintf(stderr, "Unrecognized option given, consult help");
                free(ret);
                exit(EXIT_FAILURE);
        }
    }
    ret->input = strdup(argv[argc-1]);
    return ret;
}

void option_h()
{
    printf("Print some helpful text!");
    exit(EXIT_SUCCESS);
}
void option_e();

int main(int argc, char** argv)
{
    char* message;
    char* hold;
    char* hold2;
    char* hold3;
    char* path;
    char* key;
    char* hold4;
    char* hold5;
    char* hold6;
    bool caeSuccess;
    bool xorSuccess;
    bool caexorSuccess;
    int passed;
    OPTIONS* options;
    
    #ifdef DEBUG
    {
        SHOW("COMMAND LINE OPTIONS BEFORE");
        VARSHOW("OPTION COUNT", inttoStr(argc))
        for (int i = 0; i < argc; ++i)
        {
            VARSHOW("Argument num", int)
        }
    }
    #endif

    if (argc == 0) option_h();
    options = get_options(argc,argv);
    #ifdef DEBUG
    {
        SHOW("COMMAND LINE OPTIONS ");
        VARSHOW("OPTION COUNT", inttoStr(argc))
        for (int i = 0; i < argc; ++i)
        {

        }
    }
    #endif

    
    #ifdef TEST
    {
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
    }

    //Handle options

}