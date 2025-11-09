#include <stdio.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    char c;
    struct stat* out;
    int fd, i = 0;
    out = (struct stat*) malloc(sizeof(struct stat));
    while ((c = getopt(argc,argv,"p:")) != -1)
    {
        if (c == 'p')
        {
            fd = stat(optarg, out);
        }
    }
    if (fd == -1)
    {
        printf("error\n");
    }
    else 
    {
        printf("ID of device containing file: %lu \n", out->st_dev);
        printf("Inode number: %lu \n", out->st_ino);
        printf("File type and mode :%d \n", out->st_mode);
        printf("Number of hard links :%lu \n", out->st_nlink);
        printf("User ID of owner :%d \n", out->st_uid);
        printf("Group Id of owner :%d \n", out->st_gid);
        printf("Device ID (if special file) :%lu \n", out->st_rdev);
        printf("Total size, in  bytes :%lu \n", out->st_size);
        printf("Block size for filesystem I/O :%lu \n", out->st_blksize);
        printf("Number of blocks (512-B chunks) allocated :%lu \n", out->st_blocks);
        printf("Times (all in seconds, note these are updated and there is nanosecond fields)\n");
        printf("Time of last access :%lu \n", out->st_atim.tv_sec);
        printf("Time of last modification:  :%lu \n", out->st_mtim.tv_sec);
        printf("Time of last status change :%lu \n", out->st_ctim.tv_sec);
    }
    free(out);
    out = NULL;

}
