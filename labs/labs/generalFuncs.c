#include "generalFuncs.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool globalDEBUG = false;

typedef enum{
    INT_PTR,
    CHAR_PTR,
    OPTIONS_STRUCT
}freeTypes;


void* freeAll(int numPointers, ...)
{
    va_list argv; 
    freeTypes current;
    char* hold;
    const char basic[] = " basic type ";
    const char options[] = " OPTIONS STRUCT ";
    hold = NULL;
    #ifdef DEBUG
        SHOW("INTO FREE ALL");
    #endif 

    if (globalDEBUG)
    {
        debugMSG("Freeing multiple pointers");
        debugVAR(1, "num of pointers to free", hold = intToStr(numPointers, hold));
        free(hold);
        hold = NULL;
    }
    numPointers *= 2;
    va_start(argv, numPointers);
    for (int i = 0; i < numPointers ; ++i)
    {
        current = va_arg(argv, freeTypes);
        if (globalDEBUG)
        {
            if (current != OPTIONS_STRUCT)
                debugVAR(1, hold = intToStr(i, hold), basic);
            else if (current == OPTIONS_STRUCT)
                debugVAR(1, hold = intToStr(i, hold), options);
        }
        switch (current)
        {
            case INT_PTR:
                free(va_arg(argv, int*));
                break;
            case CHAR_PTR:
                free(va_arg(argv, char*));
                break;
            case OPTIONS_STRUCT:
                deallocateOPTIONS(va_arg(argv, OPTIONS*));
                break;
        }
    }
    return NULL;
}
/*
intStringUStruct* initializeIntStringUStruct_Buffer(intStringUStruct * ret, const char* buffer)
{

    #ifdef DEBUG
        SHOW("INITIALIZING INT STRING UNION WITH BUFFER");
    #endif
    int bufferLen;
    if (buffer == NULL) return NULL;
    bufferLen = strlen(buffer) + 1;
    ret = (intStringUStruct*) malloc(sizeof(intStringUStruct));
    ret->Int_String_U_Data.buffer = (char*) malloc(bufferLen);
    ret->Int_String_U_Data.buffer = strcpy(ret->Int_String_U_Data.buffer, buffer);
    ret->UTAG = Buffer;
    return ret;
}

intStringUStruct* initializeIntStringUStruct_Size(intStringUStruct * ret, int numSize)
{
    #ifdef DEBUG
        SHOW("INITIALIZING INT STRING UNION WITH INT SIZE");
    #endif
    ret = (intStringUStruct*) malloc(sizeof(intStringUStruct));
    ret->Int_String_U_Data.size = numSize;
    ret->UTAG = Size;
    return ret;
}

void* deallocateIntStringUStruct(intStringUStruct * data)
{
    bool isString;
    if (data == NULL) return NULL; //no double free!
    isString = data->UTAG == Buffer ? true : false;
    if (isString)
    {
        if (data->Int_String_U_Data.buffer != NULL)
        {
            free(data->Int_String_U_Data.buffer);
            data->Int_String_U_Data.buffer = NULL;
        }
    }
    free(data);
    data = NULL;
    return data;
}
*/

void* deallocateOPTIONS(OPTIONS* data)
{
    bool cExist;
    bool xExist;
    //bool inputExist;
    if (data == NULL) return NULL;
    cExist = data->c_opt != NULL ? true : false;
    xExist = data->x_opt != NULL ? true : false;
    //inputExist = data->input != NULL ? true : false;

    if (cExist) free(data->c_opt);
    if (xExist) free(data->x_opt);
    //if (inputExist) free(data->input);
    //data->input = NULL;
    data->c_opt = data->x_opt = NULL;
    free(data);
    return NULL;
}

void exitFail(const char* msg, int numPointers, ...) //not kosher, doesn't cleanup after myself :(
{
    va_list argv;
    perror(msg);
    if (globalDEBUG)
        debugMSG("INTO exitFAIL");

    if (numPointers > 0)
    {
        va_start(argv, numPointers);
        for (int i = 0; i < numPointers; ++i)
        {
            free(va_arg(argv, void*));
        }
        va_end(argv);
    }
    exit(EXIT_FAILURE);

}

/*
char* handleIntMSG(char* buffer, intStringUStruct data)
{
    int msgLen;
    bool isString;
    isString = data.UTAG == Buffer ? true : false;

    //Determine type
    if (!isString)
        msgLen = data.Int_String_U_Data.size + 1;
    else if (isString)
    {
        if (data.Int_String_U_Data.buffer == NULL)
        {
            if (globalDEBUG) 
                debugERRMSG("int String Union buffer is (NULL) with bad tag");
            free(buffer);
            return NULL; //BAD!
        }
        msgLen = strlen(data.Int_String_U_Data.buffer + 1);
    }

    //Allocate size
    if (buffer != NULL)
        buffer = (char*) realloc(buffer, msgLen);
    else
        buffer = (char*) malloc(msgLen);

    //Copy if Needed
    if (isString)
    {
        buffer = strcpy(buffer, data.Int_String_U_Data.buffer);
    }

    return buffer;
}
*/

char* strredup(char* buffer, const char* msg)
{
    bool isBufferNULL;
    int msgLen;
    #ifdef DEBUG
        SHOW("INTO STRING REDUP");
    #endif
    if (msg == NULL) return buffer;
    msgLen = strlen(msg) + 1;
    isBufferNULL = buffer == NULL ? true : false;

    if (!isBufferNULL) 
        free(buffer);
    buffer = (char*) malloc(msgLen);
    return strcpy(buffer, msg);
}

char* strcatdup(char* begin, char* append, bool space)
{
    int bLength;
    int aLength;
    int sum;
    char* hold;
    if (begin == NULL)
    {
        perror("Beginning (param 1) string is uninitialized, in strdupcat");
        exit(EXIT_FAILURE);
    }
    if (append == NULL)
    {
        perror("End (param 2) string is uninitialized, in strdupcat");
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
    free(begin);
    begin = (char*) malloc(sum);
    begin = strcpy(begin, hold);
    free(hold);
    begin = strcat(begin,append);
    return begin;
}

OPTIONS* initializeOPTIONS(void)
{
    OPTIONS* ptr;
    ptr = NULL;

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
    //ptr->input = NULL;
    ptr->expected = Encrypt;
    return ptr;
}

void displayOPTIONS(OPTIONS* src)
{
    
    char trueTEXT[] = " ENABLED";
    char falseTEXT[] = " DISABLED";
    
    if (src == NULL)
        fprintf(stderr, "NO OPTIONS PROVIDED, CANNOT DISPLAY");
    else
    {
        //Note: all !'d because I messed up order of (?:), and am too lazy to fix it
        fprintf(stderr, "===OPTION STRUCTURE===\n");
        fprintf(stderr, "Encrypt:\t%s\n", (src->e ? trueTEXT: falseTEXT));
        fprintf(stderr, "Decrypt:\t%s\n", (src->d ? trueTEXT: falseTEXT));
        fprintf(stderr, "Cae-:\t%s\n", (src->c ? src->c_opt: falseTEXT));
        fprintf(stderr, "Xor:\t%s\n", (src->x ? src->x_opt: falseTEXT));
        fprintf(stderr, "Help:\t%s\n", (src->e ? trueTEXT: falseTEXT));
        fprintf(stderr, "Debug:\t%s\n", (src->e ? trueTEXT: falseTEXT));
        fprintf(stderr, "======================\n");
        fprintf(stderr, "Action to Perform: %s\n", (src->expected == 0) ? "Encrypt" : "Decrypt");
        if (src->expected == 0 || src->c)
            fprintf(stderr, "Caesar key to use: >%s<\n", src->c_opt);
        if (src->expected == 1 || src->x)
            fprintf(stderr, "Xor key to use: >%s< \n", src->x_opt);
        //fprintf(stderr, "--BEGIN INPUT STRING--\n >%s< \n--END-- INPUT STRING--\n", src->input);
        fprintf(stderr, "=====================\n");
    }
}

bool TESTCAEXOR(void)
{
    int passed;
    char* path;
    char* key;
    char* hold, *hold2, *hold3, *hold4, *hold5, *hold6;
    bool caeSuccess;
    bool xorSuccess;
    bool caexorSuccess;
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
    return (passed == 3 ? true : false);
}

void debugERRMSG(const char* msg)
{
    fprintf(stderr, "%s\nLine: %d, Function: %s, File: %s\n---\n", msg, __LINE__, __func__, __FILE__);
}

void debugMSG(const char* msg)
{
    fprintf(stderr, "%s\n", msg);
}

void debugVAR(int numArgs, ...)
{
    char * message;
    char* hold;
    va_list argv;
    numArgs *= 2; //same as << 1 :>
    message = NULL;

    VARSHOW("NUM POINTERS", message = intToStr(numArgs, message)); 
    va_start(argv, numArgs);
    if (numArgs == 1)
    {
        fprintf(stderr, "Variable %s: %s \n", va_arg(argv, char*), va_arg(argv, char*));
    }
    else
    {
        for (int i = 0; i < numArgs / 2; ++i)
        {
            hold = va_arg(argv, char*);
            fprintf(stderr, "Variable %d (%s): %s \n", (i + 1/ 2), hold, va_arg(argv, char*));
        }
    }
    va_end(argv);
    free(message);
    message = NULL;
}

char* strToCharStr(char k, char* prev)
{
    if (prev == NULL) return charToCharStr(k);
    else prev[0] = k;
    return prev;
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
    if (buffer != NULL) free(buffer);
    buffer = (char*) malloc(size);
    sprintf(buffer, "%d", num);
    return buffer;
}

char* charToCharStr(char k)
{   
    char* ret = (char*) malloc(2);
    ret[0] = k;
    ret[1] = '\0';
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
    int hold;
    int ret;
    char* message;
    message = NULL;
    checkProperKey(k);
    hold = (int) k;
    ret = hold - 32;
    SHOWVAR("ADJUSTING SPACE FROM", message = intToStr(hold, message));
    SHOWVAR("ADJUSTING SPACE TO", message = intToStr(ret, message));
    if (message != NULL) free(message);
    message = NULL;
    return ret;
}


char shiftPfromK(char p, char k, bool dir)
{
    int shiftK;
    int shiftP;
    int hold;
    char ret;
    char* msg;
    msg = NULL;
    if (p == '\n' || p == EOF || k == '\n' || k == EOF) return p;
    shiftK = keyToShift(k);
    shiftP = keyToShift(p);
    ret = 0;
    hold = 0;
    SHOWVAR("path", msg = charToCharStr(shiftP));
    SHOWVAR("key", msg = strToCharStr(shiftK, msg));
    SHOWVAR("SHIFTING", msg = strToCharStr(k, msg));
    #ifdef DEBUG
    if (dir)
        fprintf(stderr, "left\n");
    else
        fprintf(stderr, "right\n");
    #endif

    if (dir) hold = shiftP - shiftK;
    else if (!dir) hold = shiftP + shiftK;

    if (hold < 32)
    {
        SHOW("TOO LOW");
    }
    else if (hold > 126)
    {
        SHOW("TOO HIGH");
    }
    if (globalDEBUG)
        fprintf(stderr, "HOLD CALC: %d (%c) + %d (%c)= %d (%c) \n", p,p,k,k,hold,hold);
    SHOWVAR("HOLD BEFORE", msg = intToStr(hold, msg));

    hold = ((hold + 32) % 127);

    if (globalDEBUG)
        fprintf(stderr, "HOLD CALC: [[%d (%c) + %d (%c)] + 32] %% 127 = %d (%c) \n", p,p,k,k,hold,hold);
    SHOWVAR("HOLD AFTER", msg = intToStr(hold, msg));    
    free(msg);
    msg = NULL;
    checkProperKey(hold);
    ret = (char) hold;
    SHOWVAR("RESULT", msg = strToCharStr(ret, msg));
    free(msg);
    msg = NULL;
    return ret;
}

char xorFromK(char p, char k)
{
    return p ^ k;
}

OPTIONS* get_options(int argc, char** argv)
{
    int opt;
    //int optnum;
    //char * hold;
    OPTIONS* ret;
    char* message;
    message = NULL;
    if (message == NULL) 
    {
        SHOW("supress error");
    }
    
    
    SHOW("GETTING OPTIONS");
    #ifdef DEBUG
        SHOW("--OPTIONS BEGIN--");
        message = NULL;
        for (int i = 0; i < argc; ++i)
        {
            SHOWVAR(message = intToStr(i, message), argv[i]);
        }
        free(message);
        message = NULL;
        SHOW("--OPTIONS END--");
    #endif


    ret = initializeOPTIONS();

    if (argc == 1)
    {
        SHOW("NO PARAMS GIVEN, SHOWING HELP AND EXITING");
        ret->h = true;
        return ret;
    }

    while ((opt = getopt(argc,argv, ":edc:x:hD")) != -1)
    {
        switch (opt)
        {
            case 'e':
                ret->e = !ret->e;
                ret->expected = Encrypt;
                break;
            case 'd':
                ret->d = !ret->d;
                ret->expected = Decrypt;
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
    //ret->input = strdup(argv[optind]);
    return ret;
}

void option_h(void)
{
    printf("\nUSAGE ./lab1 <-options> <\"optArgs\">\n\
        -e Encrypt plaintext (this is the default action)\n\
        -d Decrypt the cipher text.\n\
        -c str The encryption string to use for the Caesar-like encryption.\n\
        -x str The encryption string to use for the xor encryption.\n\
        -h Print some helpful information about command line options and exit.\n\
        -D OPTIONAL: I use this to display some diagnostics about what is going\n\
        on inside the process. It is very useful. Remember, all diagnostic\n\
        information must go to stderr, not stdout. In fact, I recommend\n\
        you spend a little time using this option with my solution to see how\n\
        things are going.\n\
<text input+enter>\n\
[output text]\n");
    exit(EXIT_SUCCESS);
}
char* option_e(char* cInput, char* xInput, char* path)
{   
    char* hold;
    char* ret;

    #ifdef DEBUG
        SHOW("INTO OPTION ENCRYPT");
        SHOWVAR("CAE INPUT", (cInput == NULL ? "(null)" : cInput));
        SHOWVAR("XOR INPUT", (xInput == NULL ? "(null)" : xInput));
        SHOWVAR("PATH", path);
    #endif

    if (path == NULL)
    {
        if (globalDEBUG) 
            debugERRMSG("NO PATH PROVIDED :<");
        SHOW("NULL PATH");
        return NULL; //ERROR
    }

    if (cInput == NULL && xInput == NULL)
    {
        if (globalDEBUG)
            debugERRMSG("No c/x key, no changes made");
        SHOW("NO CHANGES");
        //ret = path; //<--caused double free!!
        ret = strdup(path);
    }
    else if (xInput == NULL)
    {
        if (globalDEBUG)
            debugERRMSG("No XOR key, only CAEing encrypt");
        SHOW("NO XOR");
        ret = encrypt(path,cInput,true,false);
    }
    else if (cInput == NULL)
    {
        if (globalDEBUG)
            debugERRMSG("No CAE key, only XORing encrypt");
        SHOW("NO CAE");
        ret = encrypt(path,xInput, false,true);
    }
    else 
    {
        if (globalDEBUG)
        {
            debugMSG("both CAE-XOR provided to Encrypt");
            debugVAR(3, "CAE-KEY", cInput, "XOR-KEY",xInput, "PATH", path);
        }
        hold = encrypt(path, cInput, true,false);
        ret = encrypt(hold, xInput, false, true);
        free(hold);
    }
    return ret;
}

char* option_d(char* cInput, char* xInput, char* path)
{   
    char* hold;
    char* ret;

    #ifdef DEBUG
        SHOW("INTO OPTION DECRYPT");
        SHOWVAR("CAE INPUT", (cInput == NULL ? "(null)" : cInput));
        SHOWVAR("XOR INPUT", (xInput == NULL ? "(null)" : xInput));
        SHOWVAR("PATH", path);
    #endif

    if (path == NULL)
    {
        if (globalDEBUG) 
            debugERRMSG("NO PATH PROVIDED :<");
        return NULL; //ERROR
    }

    if (cInput == NULL && xInput == NULL)
    {
        if (globalDEBUG)
            debugERRMSG("No c/x key, no changes made");
        ret = path;
    }
    else if (xInput == NULL)
    {
        if (globalDEBUG)
            debugERRMSG("No XOR key, only CAEing decrypt");
        ret = decrypt(path,cInput,true,false);
    }
    else if (cInput == NULL)
    {
        if (globalDEBUG)
            debugERRMSG("No CAE key, only XORing decrypt");
        ret = decrypt(path,xInput, false,true);
    }
    else 
    {
        if (globalDEBUG)
        {
            debugMSG("both CAE-XOR provided to decrypt");
            debugVAR(3, "CAE-KEY", cInput, "XOR-KEY",xInput, "PATH", path);
        }
        hold = decrypt(path, xInput, true,false);
        ret = decrypt(hold, cInput, false, true);
        free(hold);
    }
    return ret;
}

//Didn't know how to get EOF, manpage didn't example (or maybe i didn't read fully :<)
//**https://stackoverflow.com/questions/1428911/detecting-eof-in-c
char* getInput(char* buffer)
{
    char* hunHold;
    char* hold;
    char curr;
    int total_count;
    int curr_count;
    //int len;
    const int LIMIT = 99;
    if (feof(stdin)) 
    {
        if (buffer != NULL) free(buffer);
        buffer = NULL;
        return NULL;
    }
    //if (END != NULL) exitFail("PROGRAM DIDN\'T END AT EOF!", 1, END);

    hunHold = NULL;
    hold = NULL;
    total_count = 1;
    curr_count = 0;
    if (buffer != NULL) free(buffer);
    buffer = NULL;
    curr = getchar();
    if (curr != EOF)
        hunHold = (char*) calloc(LIMIT + 1, sizeof(char));


    while (curr != EOF)
    {
        ++total_count;
        hunHold[curr_count] = curr;
        ++curr_count;
        if (curr_count == LIMIT)
        {
            if (buffer == NULL)
            {
                buffer = (char*) calloc (LIMIT + 1, sizeof(char));
                buffer = strcpy(buffer, hunHold);
            }
            else 
            {
                hold = (char*) calloc(total_count, sizeof(char));
                hold = strcpy(hold, buffer);
                //buffer = (char*) realloc(buffer, total_count);
                free(buffer);
                buffer = hold;
                hold = NULL;
                buffer = strcat(buffer, hunHold);
            }
            free(hunHold);
            hunHold = (char*) calloc(LIMIT + 1, sizeof(char));
            curr_count = 0;
        }
        curr = getchar();
    }
    if (hunHold != NULL)
    {
        if (buffer == NULL)
            buffer = (char*) calloc(total_count, sizeof(char));
        else
        {
            hold = (char*) calloc(total_count, sizeof(char));
            hold = strcpy(hold,buffer);
            free(buffer);
            buffer = hold;
            hold = NULL;
            //buffer = (char*) realloc(buffer, total_count);
        }

        buffer = strcat(buffer, hunHold);
        //buffer[strlen(buffer)] = '\n';
    }
    else if (hunHold == NULL)
    {
        if (buffer != NULL) free(buffer);
        buffer = NULL;
        return NULL;
    }
    free(hunHold);
    hunHold = NULL;
    return buffer;
}

int main(int argc, char** argv)
{
    OPTIONS* options;
    char* retText;
    char* buff;
    char* message;
    //bool* end;
    
    message = NULL;
    options = NULL;
    retText = NULL;
    //end = NULL;
    buff = NULL;
    //Test
    #ifdef TEST
    if (globalDEBUG)
        debugMSG("INTO TEST, enable #debug for full diagnostics");
    if (TESTCAEXOR())
    {
        debugMSG("Good job! Test completed successfully");
        exit(EXIT_SUCCESS);
    }
    else 
    {
        debugMSG("Test failed, consult #debug for more info.");
        exit(EXIT_FAILURE);
    }
    #endif

 
    #ifdef DEBUG
    {
        SHOW("COMMAND LINE OPTIONS BEFORE");
        VARSHOW("OPTION COUNT", message = intToStr(argc, message));
        for (int i = 0; i < argc; ++i)
        {
            VARSHOW(message = intToStr(i, message), argv[i]);
        }
    }
    #endif

    //GET OPTIONS
    options = get_options(argc,argv);
    //#OPTION D 
    globalDEBUG = options->D; //assign option

    if (globalDEBUG)
        displayOPTIONS(options);

    //DEBUG
    #ifdef DEBUG
    {
        SHOW("COMMAND LINE OPTIONS AFTER");
        VARSHOW("OPTION COUNT", message = intToStr(argc, message));
        for (int i = 0; i < argc; ++i)
        {
            VARSHOW(message = intToStr(i, message), argv[i]);
        }
    }
    #endif

    //#Option h
    if (options->h)
    {
        free(options);
        free(message);
        options = NULL;
        option_h();
    }
    //Handle options
    
    do 
    {
        buff = getInput(buff);
        SHOWVAR("BUFFER GOTTEN", buff);
        if (!buff) SHOW("\n\n\n===BUFFER END===\n\n\n");
        else if (buff != NULL)
        {
            //#Option e
            if (options->expected == Encrypt)
            {
                if (options->d)
                    exitFail("Somehow decrypt option picked w/ encrypt chosen instead of decrypt", 2, options, buff);

                if (globalDEBUG)
                {
                    if (!options->e && !options->d)
                        debugMSG("No command line given, assuming encrypt...");
                    else if (options->e)
                        debugMSG("Encrypt option given, encrypting...");
                }
                retText = option_e(options->c_opt, options->x_opt, buff);
                #ifdef DEBUG
                    SHOW("ENCRYPT CHOSEN");
                    VARSHOW("INPUT", buff);
                    VARSHOW("OUTPUT", retText);
                #endif
            }
            //#OPTION d
            else if (options->expected == Decrypt) //if unnecessary, helps with readability
            {
                if (options->e)
                    exitFail("Somehow encrypt option picked w/ decrypt chosen instead of encrypt", 2, options, buff);
                if (!options->d)
                    exitFail("Decrypt option chosen without decrypt option set to true", 2, options, buff);
                retText = option_d(options->c_opt, options->x_opt,buff);

            #ifdef DEBUG
                SHOW("DECRYPT CHOSEN");
                VARSHOW("INPUT", buff);
                if (options->c)
                    VARSHOW("CAE USED", options->c_opt);
                else
                    VARSHOW("NO CAE USED", " ");
                if (options->x)
                    VARSHOW("XOR USED", options->x_opt);
                else
                    VARSHOW("NO XOR USED", " ");
                VARSHOW("OUTPUT", retText);
            #endif
            }

            if(globalDEBUG)
                fprintf(stderr, "\n\n\n=============\nTHE RESULT: -->%s<--\n\n\n==============\n", retText);
            write(1, retText, strlen(retText));
            
            free(retText); //somehow set to options?
            retText = NULL;
            free(message);
            message = NULL;
        }
    } while ((buff != NULL));
    //free(end);
    //end = NULL;
    options = deallocateOPTIONS(options);
    SHOW("REACHED EXIT");
    exit(EXIT_SUCCESS);
}