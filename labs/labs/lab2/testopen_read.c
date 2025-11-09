#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char c;
    char* buff;
    int fd, i = 0;
    while ((c = getopt(argc,argv,"p:")) != -1)
    {
        if (c == 'p')
        {
            fd = open(optarg, S_IWUSR);
        }
    }
    buff = (char*) calloc(2,1);

    do
    {
        c = read(fd, buff, 1);
        ++i;
        printf("\nchar %d: with byte value: <%.8B>(%c)\tbytes read: %d", i, buff[0],buff[0], c);
    }while (c != 0);
    close(fd);
    free(buff);
    buff = NULL;
}