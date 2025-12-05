#include <bsd/bsd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

/*
typedef enum {
    NONE = 000,
    READ = 100,
    WRITE = 010,
    EXECUTE = 001
} permissions;
 */

static char* filetype[] = {"block device", "character device", "directory", \
                           "FIFO/pipe", "Symbolic link -> .", "regular file", "socket", "unknown?"};
static char symLinkFollow[100];
#define BLOCK 0
#define CHAR 1
#define DIR 2
#define FIPI 3
#define SYML 4
#define REG 5
#define SOCK 6
#define UNKN 7

#define symlNULL "Symbolic link - with dangling destination"



#define prefix1 "File: "
#define prefix2 "  File type:                "
#define prefix3 "  Device ID number:         "
#define prefix4 "  I-node number:            "
#define prefix5 "  Mode:                     "
        //(700 in octal)
#define prefix5_2 "       (%3s in octal)\n"
#define prefix6 "  Link count:               "
#define prefix7 "  Owner Id:                 "
#define prefix7_2 "(UID = %d)\n"
#define prefix8 "  Group Id:                 "
#define prefix8_2 "(GID = %d)\n"
#define prefix9 "  Preferred I/O block size: "
#define prefix10 "  File size:                "
#define prefix11 "  Blocks allocated:         "
#define prefix12 "  Last file access:         "
#define prefix13 "  Last file modification:   "
#define prefix14 "  Last status change:       "
#define prefix12 "  Last file access:         "
#define prefix13 "  Last file modification:   "
#define prefix14 "  Last status change:       "
#define ext_epoch " (seconds since the epoch)\n"
#define ext_local " (local)\n"
#define ext_gmt   " (GMT)\n"


void no_arg_exit(void);
void fail_stat_skip(char name[]);
int getFileType(mode_t file);
const char* follow_link(char name[]);
void printStat(char name[], struct stat * file);
const char* getType(mode_t mode, char name[]);


void no_arg_exit(void)
{
    fprintf(stderr, "Usage: ./mystat <filename1> ... <filenameN>\n");
    exit(EXIT_FAILURE);
}
void fail_stat_skip(char name[])
{
    fprintf(stderr, "*** Failed to stat file '%s', skipping\n***  File '%s' does not exist or we done have access.\n*** Could not stat file: No such file or directory\n",name ,name);
}

int getFileType(mode_t file)
{
    switch (file & S_IFMT)
    {
        case S_IFBLK: return BLOCK;
        case S_IFCHR: return CHAR;
        case S_IFDIR: return DIR;
        case S_IFIFO: return FIPI;
        case S_IFLNK: return SYML;
        case S_IFREG: return REG;
        case S_IFSOCK: return SOCK;
    }
    return UNKN;
}

const char* follow_link(char name[])
{
    char* hold;
    char carry[100];
    int count = 0;
    memset(symLinkFollow, 0, 100);
    memset(carry, 0, 100);

    strcpy(symLinkFollow, filetype[SYML]);
    if (count == -1) //WHAT HAPPENED?
    {
        perror("I honestly don't even know. Somehow PRINTING failed?");
        exit(EXIT_FAILURE);
    }

    hold = realpath(name, carry);
    if (hold == NULL) return symlNULL;
    count = readlink(name, symLinkFollow + 17, 81);
    if (count == -1) {return symlNULL;}
    return symLinkFollow;
}
//#define follow(link) (follow_link(link) != NULL ? symLinkFollow : symlNULL)
//#define getType(mode,point) (getFileType(mode) == SYML ? follow(point) : filetype[getFileType(mode)])

#define permPrint(num,numHold,from) {if ((snprintf(numHold, 9, "%7.3o", from)) == -1) {perror("something went wrong in printing?"); exit(EXIT_FAILURE);} \
                                if ((snprintf(num,4,"%3s", numHold + 4)) == -1){perror("something went wrong in printing, the second?"); exit(EXIT_FAILURE);}}

const char* getType(mode_t mode, char name[])
{
    int ret_val = getFileType(mode);
    if (ret_val == SYML)
        return follow_link(name);
    return filetype[ret_val];
}

void printStat(char name[], struct stat * file)
{
    struct passwd* pass;
    struct group*  grp;
    struct tm timeaccess, timemod, timestatus;
    char mode[12] = "------------";
    char permHold[9] = "0000";
    char perms[4] = "000";
    char access[45], modify[45], status[45];

    printf("%s%s\n", prefix1,name);
    printf("%s%s\n", prefix2, getType(file->st_mode, name));
    printf("%s%ld\n", prefix3, file->st_dev);
    printf("%s%ld\n", prefix4, file->st_ino);
    strmode(file->st_mode, mode);
    permPrint(perms,permHold,file->st_mode);
    printf("%s%s" prefix5_2, prefix5, mode, perms);
    printf("%s%ld\n",prefix6, file->st_nlink);
    
    pass = getpwuid(file->st_uid);
    grp = getgrgid(file->st_gid);
    printf("%s%-18s" prefix7_2, prefix7, pass->pw_name, pass->pw_uid);
    printf("%s%-18s" prefix8_2, prefix8, grp->gr_name, grp->gr_gid);
    printf("%s%ld bytes\n",prefix9,file->st_blksize);
    printf("%s%ld bytes\n", prefix10, file->st_size);
    printf("%s%ld\n", prefix11, file->st_blocks);
    printf("%s%10ld" ext_epoch , prefix12,file->st_atim.tv_sec);
    printf("%s%10ld" ext_epoch, prefix13, file->st_mtim.tv_sec);
    printf("%s%10ld" ext_epoch,prefix14, file->st_ctim.tv_sec);
    
    localtime_r((&(file->st_atim.tv_sec)), &timeaccess);
    localtime_r(&file->st_mtim.tv_sec, &timemod);
    localtime_r(&file->st_ctim.tv_sec, &timestatus);

    memset(access, 0, 45);
    memset(modify,0, 45);
    memset(status,0,45);

    strftime(access, 44, "%Y-%m-%d %H:%M:%S %z (%Z) %a",&timeaccess);
    strftime(modify, 44, "%Y-%m-%d %H:%M:%S %z (%Z) %a",&timemod);
    strftime(status, 44, "%Y-%m-%d %H:%M:%S %z (%Z) %a",&timestatus);

    printf("%s%s" ext_local, prefix12, access);
    printf("%s%s" ext_local, prefix13, modify);
    printf("%s%s" ext_local, prefix14, status);

//#define frees 
    //{free(timeaccess); 
     //free(timemod); 
     //free(timestatus); 
    //timeaccess = timemod = timestatus = NULL;}

    memset(access, 0, 45);
    memset(modify,0, 45);
    memset(status,0,45);

    gmtime_r(&file->st_atim.tv_sec, &timeaccess);
    gmtime_r(&file->st_mtim.tv_sec, &timemod);
    gmtime_r(&file->st_ctim.tv_sec, &timestatus);
    strftime(access, 44, "%Y-%m-%d %H:%M:%S %z (%Z) %a",&timeaccess);
    strftime(modify, 44, "%Y-%m-%d %H:%M:%S %z (%Z) %a",&timemod);
    strftime(status, 44, "%Y-%m-%d %H:%M:%S %z (%Z) %a",&timestatus);


    printf("%s%s" ext_gmt, prefix12, access);
    printf("%s%s" ext_gmt, prefix13, modify);
    printf("%s%s" ext_gmt, prefix14, status);

    //free(timeaccess);
    //free(timemod);
    //free(timestatus);
    //free(pass);
    //free(grp);
    pass = NULL;
    grp = NULL;

}

int main(int argc, char* argv[])
{
    int error;
    struct stat file;
    error = 0;
    if (argc <= 1) no_arg_exit();
    for (int i = 1; i < argc; ++i)
    {
        error = lstat(argv[i], &file);
        if (error == -1) fail_stat_skip(argv[i]);
        else printStat(argv[i], &file);
        memset(&file, 0, sizeof(file));
        memset(&file.st_atim, 0, sizeof(file.st_atim));
        memset(&file.st_mtim, 0, sizeof(file.st_mtim));
        memset(&file.st_ctim, 0, sizeof(file.st_ctim));
    }
    exit(EXIT_SUCCESS);
}