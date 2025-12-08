//**------------------------------[INFORMATION]-------------------------------//
/*
 * Molly Diaz, modiaz@pdx.edu, 966934367, CS333 TR 1:30-3:10
 * Lab 6, rockem-sockets  (CLIENT)
 *
 */
//**------------------------------[--INCLUDES-]-------------------------------//

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// read the comments in this file.
#include "../rockem_hdr.h"
//**------------------------------[--DEFINES--]-------------------------------//
#define VERBLVLS 3
#define VERBSIZE 100
//**------------------------------[--GLOBALS--]-------------------------------//

static short is_verbose = 0;
static int usleep_time = 0;
static char ip_addr[50] = {DEFAULT_IP};
static in_addr_t ip_addr_int = DEFAULT_IP_INT;
static short ip_port = DEFAULT_SERVER_PORT;
static char verbose_ch[VERBLVLS][VERBSIZE];

//**------------------------------[PROTOTYPES-]-------------------------------//

int decipher_cmd(char *);
int get_socket(char *, int);
void get_file(char *);
void put_file(char *);
void list_dir(void);
void *thread_get(void *);
void *thread_put(void *);

//**------------------------------[DEFINITIONS]-------------------------------//

//============[----MAIN---]===========//
int main(int argc, char *argv[]) {
  pthread_attr_t attr;
  cmd_t cmd;
  int command;
  pthread_t *threads;
  int i, j;
#define VERBMAX(i) min(i, VERBLVLS)
#define CLRVRB CLEARCSTRS(verbose_ch, VERBSIZE, VERBLVLS)
  CLRVRB;
  memset(&cmd, 0, sizeof(cmd_t));
  {
    int opt = 0;

    while ((opt = getopt(argc, argv, CLIENT_OPTIONS)) != -1) {
      switch (opt) {
      case 'i':
        SAY("INTO option I");
        ADD_VERB(verbose_ch, ";ip_addr->, ", 1);
        strncpy(ip_addr, optarg, 50);
        SHOWs("ip_addr now ", ip_addr);
        ip_addr_int = inet_addr(ip_addr);
        SHOWi("IP ADDRESS AS INT ", ip_addr_int);
        // copy optarg into the ip_addr
        break;
      case 'p':
        SAY("INTO OPTION p");
        VERB_ADD(verbose_ch, ";change port-> ", 1);
        VERB_ADD(verbose_ch, optarg, 1);
        ip_port = atoi(optarg);
        SHOWi("new ip port ", ip_port);
        // CONVERT and assign optarg to ip_port
        break;
      case 'c':
        SAY("INTO OPTION C");
        VERB_ADD(verbose_ch, ";command -> ", 1);
        VERB_ADD(verbose_ch, optarg, 1);
        strncpy(cmd.cmd, optarg, 6);
        SHOWs("command copied ", cmd.cmd);
        // copy optarg into data member cmd.cmd
        break;
      case 'v':
        SAY("INTO OPTION V");
        is_verbose++;
        is_verbose = VERBMAX(is_verbose);
        SHOWi("NEW VERBOSE VALUE ", is_verbose);
        break;
      case 'u':
        SAY("INTO OPTION U");
        ++usleep_time;
        SHOWi("new usleep ", usleep_time * USLEEP_INCREMENT);
        // add 1000 to usleep_time
        break;
      case 'h':
        SAY("HELP AND EXIT");
        fprintf(stderr, "%s ...\n\tOptions: %s\n", argv[0], CLIENT_OPTIONS);
        fprintf(stderr, "\t-i str\t\tIPv4 address of the server (default %s)\n",
                ip_addr);
        fprintf(
            stderr,
            "\t-p #\t\tport on which the server will listen (default %hd)\n",
            DEFAULT_SERVER_PORT);
        fprintf(stderr, "\t-c str\t\tcommand to run (one of %s, %s, or %s)\n",
                CMD_GET, CMD_PUT, CMD_DIR);
        fprintf(stderr,
                "\t-u\t\tnumber of thousands of microseconds the client will "
                "sleep between read/write calls (default %d)\n",
                0);
        fprintf(stderr, "\t-v\t\tenable verbose output. Can occur more than "
                        "once to increase output\n");
        fprintf(stderr, "\t-h\t\tshow this rather lame help message\n");
        exit(EXIT_SUCCESS);
        break;
      default:
        fprintf(stderr, "*** Oops, something strange happened <%s> ***\n",
                argv[0]);
        break;
      }
    }
  }
  command = decipher_cmd(cmd.cmd);
  VERB_PRINT(verbose_ch, is_verbose);
  CLRVRB;
  if (command == UNKNOWN) {
    fprintf(stderr, "ERROR: unknown command >%s< %d\n", cmd.cmd, __LINE__);
    exit(EXIT_FAILURE);
  } else if (command == DIR) {
    VERB_ADD(verbose_ch, "DIRECTORY COMMAND FOUND, ARGUMENTS IGNORED", 1);
    SAY("DIRECTORY");
    list_dir();
  } else {
    threads = (pthread_t *)malloc(sizeof(pthread_t) * (argc - optind));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    j = 0;
    for (i = optind; i < argc; ++i) {
      VERB_ADD(verbose_ch, "-- NEW THREAD --\n", 2);
      switch (command) {
      case GET:
        pthread_create(&threads[j], &attr, thread_get, (void *)argv[i]);
        break;
      case PUT:
        pthread_create(&threads[j], &attr, thread_put, (void *)argv[i]);
        break;
      }
      ++j;
    }
  }
  // process the files left on the command line, creating a threas for
  // each file to connect to the server

  pthread_exit(NULL);
}

//============[---OTHER---]===========//
int decipher_cmd(char *cmd) {
  IF_GET(cmd) return GET;
  IF_PUT(cmd) return PUT;
  IF_DIR(cmd) return DIR;
  return 0;
}

int get_socket(char *addr, int port) {
  // configure and create a new socket for the connection to the server
  int sockfd, connect_status, sockopt [[maybe_unused]];
  // int force = 1;
  struct sockaddr_in servaddr;

  SAY("GETTING SOCKET");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  SAY("SOCKETTTED");
  IF_FAIL(sockfd, "Failed to socket self")
  memset(&servaddr, 0, sizeof(servaddr));
  // more stuff in here
  // HEAVILY CITED: https://www.geeksforgeeks.org/c/socket-programming-cc/
  // setsockopt(sockfd,)

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = inet_addr(addr);

  SAY("CONNECTING");
  connect_status =
      connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  SAY("CONNECTED");

  IF_FAIL(connect_status, "Failed to connect socket to addr")

  return (sockfd);
}

// get one file
void get_file(char *file_name) {
  cmd_t cmd;
  int fd;
  ssize_t sent;
  ssize_t bytes_read;
  char buffer[MAXLINE];

  strcpy(cmd.cmd, CMD_GET);
  if (is_verbose) {
    fprintf(stderr, "next file: <%s> %d\n", file_name, __LINE__);
  }
  strcpy(cmd.name, file_name);
  if (is_verbose) {
    fprintf(stderr, "get from server: %s %s %d\n", cmd.cmd, cmd.name, __LINE__);
  }

  // get the new socket to the server (get_socket(...)
  cmd.sock = get_socket(ip_addr, ip_port);
  // write the command to the socket
  sent = send(cmd.sock, &cmd, sizeof(cmd), 0);
  if (sent < (long)sizeof(cmd)) {
    SHOWd("NUMBER SENT", sent);
    fprintf(stderr, "couldn't send command? is server down..?, EXITING");
    exit(EXIT_FAILURE);
  }
  // open the file to write
  fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
  IF_FAIL(fd, "Coudln't create file? I HAVE NO IDEA!");
  do {
    bytes_read = read(cmd.sock, buffer, MAXLINE);
    if (bytes_read == 0)
      break;
    if (bytes_read == -1) {
      fprintf(stderr, "CONNECTION CLOSED SUDDENLY! UH OH! EXITING!");
      exit(EXIT_FAILURE);
    }
    sent = write(fd, buffer, bytes_read);
    IF_FAIL(sent, "Couldn't write to local file? IM GETTING A HEADACHE!");
  } while (bytes_read != 0);
  // loop reading from the socket, writing to the file
  //   until the socket read is zero
  close(fd);
  close(cmd.sock);
  // close the file
  // close the socket
}

void put_file(char *file_name) {
  cmd_t cmd;
  int fd;
  ssize_t bytes_wrote;
  ssize_t bytes_read;
  char buffer[MAXLINE];
  SHOWs("PUTTING ", file_name);

  strcpy(cmd.cmd, CMD_PUT);
  if (is_verbose) {
    fprintf(stderr, "next file: <%s> %d\n", file_name, __LINE__);
  }
  strcpy(cmd.name, file_name);
  if (is_verbose) {
    fprintf(stderr, "put to server: %s %s %d\n", cmd.cmd, cmd.name, __LINE__);
  }
  cmd.sock = get_socket(ip_addr, ip_port);

  // get the new socket to the server (get_socket(...)
  fd = open(file_name, O_RDWR);
  IF_FAIL(fd, "\nCouldn't open file to send.. AY CARUMBA! (i can't speel)")
  // write the command to the socket
  bytes_wrote = send(cmd.sock, &cmd, sizeof(cmd), 0);
  if (bytes_wrote < (long)sizeof(cmd)) {
    fprintf(stderr, "Couldn't get through socket to send command :(-- AAAAA");
    exit(EXIT_FAILURE);
  }
  // open the file for read
  do {
    bytes_read = read(fd, buffer, MAXLINE);
    if (bytes_read == 0)
      close(cmd.sock);
    else if (bytes_read == -1) {
      fprintf(stderr, "SOMETHING WENT WRONG TRYING TO READ FILE");
      exit(EXIT_FAILURE);
    } else {
      bytes_wrote = send(cmd.sock, buffer, bytes_read, 0);
      if (bytes_wrote == 0) {
        fprintf(stderr, "not sure if error...butttt sent 0 bytes? for put?");
        close(cmd.sock);
      } else
        IF_FAIL(bytes_wrote, "couldn't send my file :(");
    }
  } while (bytes_read != 0 && bytes_wrote != 0);

  // loop reading from the file, writing to the socket
  //   until file read is zero

  close(fd);
  // close the file
  // close the socket
}

void list_dir(void) {
  cmd_t cmd;
  int sockfd;
  ssize_t bytes_read, bytes_wrote;
  char buffer[MAXLINE];

  printf("dir from server: %s \n", cmd.cmd);

  // get the new socket to the server (get_socket(...)
  sockfd = get_socket(ip_addr, ip_port);

  strcpy(cmd.cmd, CMD_DIR);
  // write the command to the socket
  bytes_read = send(sockfd, CMD_DIR, strlen(CMD_DIR), 0);
  IF_FAIL(bytes_read, "\nCOULDNT SEND DIRECTORY FROM SOCKET >:(");

  // loop reading from the socket, writing to the file
  //   until the socket read is zero
  do {
    bytes_read = read(sockfd, buffer, MAXLINE);
    if (bytes_read == 0)
      break;
    IF_FAIL(bytes_read, "COULDNT READ DIRECTORY FROM SOCKET *~*");
    bytes_wrote = write(STDOUT_FILENO, buffer, bytes_read);
    IF_FAIL(bytes_wrote,
            "couldn't write directory to stdout..? SOMETHING IS VERY WEIRD");
  } while (bytes_read != 0);
  close(sockfd);
  // close the socket
}

void *thread_get(void *info) {
  char *file_name = (char *)info;
  pthread_t tid [[maybe_unused]] = pthread_self();
  SHOWd("GET THREAD ", tid);
  // pthread_detach(tid);
  //  detach this thread 'man pthread_detach' Look at the EXMAPLES

  // process one file
  get_file(file_name);

  SHOWd("EXIT GTHREAD ", tid);
  pthread_exit(NULL);
}

void *thread_put(void *info) {

  char *file_name = (char *)info;
  pthread_t tid = pthread_self();
  SHOWd("PUT THREAD ", tid);
  pthread_detach(tid);
  // detach this thread 'man pthread_detach' Look at the EXMAPLES

  // process one file
  put_file(file_name);

  SHOWd("EXIT PTHREAD ", tid);
  pthread_exit(NULL);
}
