#include <stdbool.h>
#include <stdio.h>
//#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* getOpt(void)
{
    return NULL;
}

int main(void)
{
    int length;
    int keyI;
    int keyL;
    //char* buff;
    const char buff[] = "abcdefghijklmnopqrstuvwxyz";
    const char key[] = " #";
    length = strlen(buff);
    keyL = strlen(key);
    char* out;
    out = (char*) calloc(length + 1, sizeof(char));

    for (int i = 0, k = 0, k2 = 1;i < length / 2; i += 2, k %= keyL, k2  %= keyL)
    {
        out[i] = shiftPfromK(buff[i], key[k]);
        out[i + 1] = shiftPfromK(buff[i + 1], key[k2]);
        ++k;
        ++k2;
    }
    printf("before:%s\nafter :%s\n", buff, out);
    
}
//aebdhegkhjnkmqnptqswtvzwy}
//aebghigkhmnomqnstuswtyz{y}
//aebdhegkhjnkmqnptqswtvzwy}