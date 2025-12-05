#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <time.h>
#include "thread_hash.h"
#define SPACE_ARRAY_SIZE 100
static char space_array[SPACE_ARRAY_SIZE] = "                                                                                                   ";
void test_chars()
{
    char c;
    char* c_ptr;
    char* c_ptr_arr[3];
    //char** c_ptr_ptr_arr[3];
    c = 'c';
    c_ptr = (char*) malloc(sizeof(char) * 4);
    c_ptr = "ptr";
    c_ptr_arr[0] = strdup("c ptr arr 1");
    c_ptr_arr[1] = strdup("c ptr arr 2");
    c_ptr_arr[2] = strdup("c ptr arr 3");


    printf("char %c\nchar* %s\nchar*[] %s\t%s\t%s", c, c_ptr, c_ptr_arr[0], c_ptr_arr[1], c_ptr_arr[2]);
}

void test_ptr(int* ptr)
{
    *ptr = 2;
}

void test_ptr_main(void)
{
    int this = 0;
    printf("THIS IS ZERO, VALUE: %d\n", this);
    test_ptr(&this);
    printf("THIS IS TWO? VALUE: %d\n", this);
    if (this == 0) exit(EXIT_FAILURE);
}

int crypt_test(void)
{
    char* plain = "plainname";
    char* hash = "6W/YGO82Jesqw";
    char* ret = NULL;
    struct crypt_data cry;

    memset(&cry,0,sizeof(cry));
    printf("PLAIN BEFORE: %s\tHASH BEFORE: %s\n", plain, hash);
    strncpy(cry.input, plain, CRYPT_MAX_PASSPHRASE_SIZE);
    strncpy(cry.setting, hash, CRYPT_OUTPUT_SIZE);
    ret = crypt_rn(plain, hash,&cry, sizeof(cry));

    printf("\nAfter:\nPlain: %s\tHash: %s\t\ncryIn: %s\tcrySet: %s\tcryOut %s\n",plain,hash,cry.input,cry.setting,cry.output);
    exit(EXIT_SUCCESS);
}

int length_num(int num)
{
    int places;
    places = 0;
    while ((num = num / 10) != 0)
        ++places;
    if (places == 0) return 1;
    return places;
}

int test_length_num(void)
{
    int curr = 1;
    int curr_d = 1;
    int curr_e = 1;
    int nums[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    for (int i = 0; i < 10; ++i)
    {
        if (i == 0) curr = 1;
        else curr *= 10;
        curr_e = curr / nums[i];
        curr_d = length_num(nums[i]);
        printf("Number expected: %d\tNumber derived: %d\n", curr,curr_d);
    }
    return 0;
}

void weird_width_math(void)
{
    int hold;
    char hold_c;
    int hold_r;
    int x = 1023;
    char spaces[10] = "---------";
    char hello[6] = "hello";
    char num[10]  = "123456789";
    for (int i = 0, j = 4; i < 6; ++i, --j)
    {
        printf("character: %d: %c\n", i, hello[j]);
    }
    printf("demonstration\n");
    for (int j = 8; j >= 0; --j)
    {
        printf("%c", num[j]);
    }
    printf("\nIn practice, before: %d\nafter: ", x);
    hold = length_num(x);
    char* hold_s = (char*) malloc(hold + 1);
    hold_r = 9 - hold;
    sprintf(hold_s, "%d", x);
    for (int i = 0; i < hold_r; ++i)
    {
        printf("%c", spaces[i]);
    }
    for (int i = 0; i < hold + 1; ++i)
    {
        printf("%c", hold_s[i]);
    }
    printf("\n");
    free(hold_s);
    hold_s = NULL;
}

void easier_width(void)
{
    int digit_length[] = {1,10,100,1000,10000};
    printf("COLUMNS:\n12345\n|||||\n");
    for (int i = 0; i < 5; ++i)
    {
        printf("%*d\n", 5, digit_length[i]);
    }
}

char* print_spaces(int num)
{
    return (space_array + (SPACE_ARRAY_SIZE - 1 - num));
}

void CS333_TEST_OUT(void)
{
    unsigned int number = time(NULL);
    srand(number);

    
    printf("\n");
    printf("thread:%*d ", 3, rand() % 1000);
    printf("%*.*lf sec%s", 6,2,((double) (rand() % 1000000) / 10),print_spaces(14));
    printf("DES: %*d%s",5, rand() % 10000, print_spaces(15));
    printf("NT: %*d%s",5, rand()% 10000, print_spaces(14));
    printf("MD5: %*d%s",5, rand()% 10000, print_spaces(11));
    printf("SHA256: %*d%s",5, rand()% 10000, print_spaces(11));
    printf("SHA512: %*d%s",5, rand()% 10000, print_spaces(9));
    printf("YESCRYPT: %*d%s",5, rand()% 10000, print_spaces(4));
    printf("GOST_YESCRYPT: %*d%s",5, rand()% 10000, print_spaces(11));
    printf("BCRYPT: %*d%s",5, rand() % 100000, print_spaces(2));
    printf("total: %*d  ", 8, rand() % 100000000);
    printf("failed: %*d\n", 8, rand()% 100000000);
}

//->total written
int verbose_add(thread_data * given)
{
    int thread, des, nt, md5, sha256, sha512, yes, gyes, b, total, failed;
    double time;
    time = given->time;
    thread = given->thread_num % 1000;
    des = given->encounters[DES] % 10000;
    nt = given->encounters[NT]% 10000;
    md5 = given->encounters[MD5]% 10000;
    sha256 = given->encounters[SHA256]% 10000;
    sha512 = given->encounters[SHA512]% 10000;
    yes = given->encounters[YESCRYPT]% 10000;
    gyes = given->encounters[GOST_YESCRYPT]% 10000;
    b = given->encounters[BCRYPT]% 10000;
    total = given->total % 100000000;
    failed = given->failures % 100000000;
    snprintf(given->verbose, VERBOSE_LENGTH, "thread:%3d %6.2lf sec              DES: %5d               NT: %5d              MD5: %5d           SHA256: %5d           SHA512: %5d         YESCRYPT: %5d    GOST_YESCRYPT: %5d           BCRYPT: %5d  total: %8d  failed: %8d\n", thread , time, des , nt , md5 , sha256 , sha512 , yes , gyes , b , total, failed); 
    return total;
}

int test_verbose(void)
{
    CS333_TEST_OUT();
    thread_data data;
    for (int i = 0; i < ALGORITHM_MAX; ++i)
    {
        data.encounters[i] = rand();
    }
    data.time = (double) (rand() % 100000);
    data.failures = rand();
    data.total = rand();
    int hold = verbose_add(&data);
    printf("total returned = %d\n", hold);
    printf("%s", data.verbose);
    return 0;
}

void test_fors(void)
{
    int i = 0;
    for (i = 1; i < 10; ++i)
    {
        continue;
    }
    printf("value of i before: 0, after: %d", i);
}
char shiftleft(char path, char key) {
  int LANGSIZE = 94;
  int LANGLOW = 32;
  int diff;
  path -= LANGLOW;
  key -= LANGLOW;
  diff = path - key;

  return ((diff < 0 ? LANGSIZE + 1 - diff : diff) + LANGLOW);
}

char shiftright(char p, char k) {
  int LANGLOW = 32;
  int LANGSIZE = 94;
  p -= LANGLOW;
  k -= LANGLOW;
  return (((p + k) % (LANGSIZE + 1)) + LANGLOW);
}



int test_cae()
{
    const char string[] = "abcdefghijklmnopqrstuvwxyz";
    const char string_key[] = " !\"#$%&'()*+,-./0123456789:";
    char string_out[] = "--------------------------";
    for (int i = 0; i < strlen(string); ++i)
    {
        printf("Moving alphabet %d chars left (%c)\n", i,string_key[i]);
        printf("%s\n||||||||||||||||||||||||||\n%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\t(%d)\n||||||||||||||||||||||||||\n", string ,string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i] - 32);
        for (int j = 0; j < strlen(string); ++j)
        {
            string_out[j] = shiftleft(string[j], string_key[i]);
        }
        printf("%s\n-----\n",string_out);

        printf("Moving alphabet %d chars right (%c)\n", i,string_key[i]);
        printf("%s\n||||||||||||||||||||||||||\n%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\t(%d)\n||||||||||||||||||||||||||\n", string ,string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i],string_key[i] - 32);
        for (int j = 0; j < strlen(string); ++j)
        {
            string_out[j] = shiftright(string[j], string_key[i]);
        }
        printf("%s\n\n",string_out);
        
    }
    return 0;
}

int bitmasking(void)
{
    char a,b,c,d,e,f;
    int ai,bi,ci,di,ei,fi;
    char c_array[] = {'a','b','c','d', 'e', 'f'};
    int i_array[]  = {100,010, 001,200, 400, 700};
    for (int i = 0; i < 6; ++i)
    {
        printf("\nCharacter 1: %c  |  Character 2: %c\n--->%c", c_array[i], c_array[(i + 1) % 6], c_array[i] | c_array[(i + 1) %6]);
    }
    printf("\nNumber modification:\n100 | 010 | 001 : %d\n100 | 200 | 400 : %d\n", (i_array[0] | i_array[1] | i_array[2]), (i_array[3] | i_array[0] | i_array[4]));
    return 0;
}

void fuckyou(void)
{
    char big[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    for (int i = 0; i < 96;++i)
    {
        printf("%s", big + i);
        printf("%.*s\n", i, big);
    }
}

void print10000seq(void)
{
    int j = 0;
    printf("INTO PRINT 1000\n");
    for (int i = 0 ; i < 10001; ++i)
    {
        printf("%d", i % 10);
        if (j == 50)
        {
            printf("\t#%d\n", i);
            j = 0;
        }
        ++j;
    }
    printf("\n");

}

void printbyteVal(char a, char b)
{
    printf("Value of (%d): %0.8B\n", a,a);
    if (a != b)
        printf("Value of (%d): %0.8B\n", b,b);
}

int main(void)
{
    printbyteVal('\n', '\t');
}