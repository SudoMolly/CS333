//gcc -Wall -o signals signals.c

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define DELAY 2
#define NUM_FORK 4

static volatile sig_atomic_t alarm_count = 5;
static volatile sig_atomic_t num_fork = 0;

//no choice in these..
void sigint_handler(int);
void sigchld_handler(int);
void alarm_handler(int);

int main(void)
{
    signal(SIGALRM,alarm_handler); //depricated, sigaction is newer
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    alarm(DELAY);
    while(alarm_count) pause();
    raise(SIGINT); //alternatively kill(getpid(),SIGINT) 
    {
        pid_t cpid = 0;
        for (int i = 0; i < NUM_FORK; ++i)
        {
            cpid = fork();
            if (cpid == 0) execlp("sleep","sleep","2",NULL);
            ++num_fork;
        }
        while(1) pause();

    }


    return (EXIT_SUCCESS);
}

void alarm_handler(__attribute__((unused))int sig)
{
    time_t t = 0;
    --alarm_count;
    t = time(NULL);
    printf("The time is: %s", ctime(&t));
    printf(" %d calls remaining\n", alarm_count);
    if (alarm_count) alarm(DELAY);
}

void sigint_handler(int sig)
{
    printf("Signal for %d caught\n", sig);
    printf("I will never die! I will never die!\n");
}

void sigchld_handler(int sig)
{
    int status;
    pid_t cpid;
    while ((cpid = waitpid(-1,&status,WNOHANG)) > 0)
    {
        printf("\nparent signal handler: found child exit %d:\t pid: %d\texit value: %d\n",
                sig, cpid, WEXITSTATUS(status));
        --num_fork;
    }
    if(num_fork == 0) printf("all child processes reaped, control \\ to exit\n");

}
