#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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