//**------------------------------[INFORMATION]-------------------------------//
/*
 * Molly Diaz, modiaz@pdx.edu, 966934367, CS333 TR 1:30-3:10
 * Lab 6, rockem-sockets  (SERVER)
 *
 */
//**------------------------------[--INCLUDES-]-------------------------------//

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
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

#define LISTENQ 100

//**------------------------------[--GLOBALS--]-------------------------------//

static short is_verbose = 0;
static int usleep_time = 0;
static long tcount = 0;
static int current_connections = 0;
static pthread_mutex_t connections_mutex = PTHREAD_MUTEX_INITIALIZER;
static char verbose_ch[3][100];
static pthread_t *tids[MAXCONNECT];
static int tids_t[MAXCONNECT];

//**------------------------------[PROTOTYPES-]-------------------------------//

void EMERGENCY_EXIT(void);
void process_connection(int sockfd, void *buf, int n);
void *thread_get(void *p);
void *thread_put(void *p);
void *thread_dir(void *p);
void *server_commands(void *p);
void current_connections_inc(void);
void current_connections_dec(void);
unsigned int current_connections_get(void);
void server_help(void);
//**------------------------------[DEFINITIONS]-------------------------------//

//============[----MAIN---]===========//
int main(int argc, char *argv[]) {
  int sock_err, sockopt_err, bind_err, listen_err;
  int force = 1;
  int listenfd = 0;
  int sockfd = 0;
  int n = 0;
  int opt = 0;
  short ip_port = DEFAULT_SERVER_PORT;
  pthread_attr_t *attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t));
  char buf[MAXLINE];
  socklen_t clilen;
  struct sockaddr_in cliaddr;
  struct sockaddr_in servaddr;
  pthread_t cmd_thread;

#define VERBMAX(i) min(i, 3)
#define CLRBUF CLEARCSTRS(buf, MAXLINE, 1)
#define CLRVRB CLEARCSTRS(verbose_ch, 100, 3)
  CLRVRB
  CLRBUF
  // options
  {
    ADD_VERB(verbose_ch, "OPTIONS START", 3);
    while ((opt = getopt(argc, argv, SERVER_OPTIONS)) != -1) {
      switch (opt) {
      case 'p':
        // CONVERT and assign optarg to ip_port
        // strcat(verbose_ch[0], ";change port-> ");
        // strcat(verbose_ch[0], optarg);
        SAY("INTO OPTION p");
        ADD_VERB(verbose_ch, "\nchange port-> ", 3);
        ADD_VERB(verbose_ch, optarg, 3);
        ip_port = atoi(optarg);
        SHOWi("new ip port ", ip_port);
        break;
      case 'u':
        SAY("INTO OPTION U");
        ++usleep_time;
        SHOWi("NEW SLEEP TIME ", usleep_time * USLEEP_INCREMENT);
        // add 1000 to usleep_time
        break;
      case 'v':
        SAY("INTO OPTION V");
        ADD_VERB(verbose_ch, "\nVerbose++", 3);
        is_verbose++;
        is_verbose = VERBMAX(is_verbose);
        SHOWi("NEW VERBOSE VALUE ", is_verbose);
        break;
      case 'h':
        fprintf(stderr, "%s ...\n\tOptions: %s\n", argv[0], SERVER_OPTIONS);
        fprintf(
            stderr,
            "\t-p #\t\tport on which the server will listen (default %hd)\n",
            DEFAULT_SERVER_PORT);
        fprintf(stderr,
                "\t-u\t\tnumber of thousands of microseconds the server will "
                "sleep between "
                "read/write calls (default %d)\n",
                usleep_time);
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
    VERB_PRINT(verbose_ch, is_verbose);
    CLRVRB

    {
      int host_err, entry_err [[maybe_unused]], ip_err [[maybe_unused]];
      char hostname[256];
      struct hostent *host_entry = NULL;
      char *IPbuffer = NULL;

      // need to ignore depreciated warnings, I HAVE NO IDEA HOW ELSE TO DO!
#pragma GCC diagnostic ignored "-Wall"
      memset(hostname, 0, sizeof(hostname));
      host_err = gethostname(hostname, sizeof(hostname));
      IF_FAIL(host_err, "COULD NOT GET HOSTNAME")
      host_entry = gethostbyname(hostname);
      IPbuffer = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));
#pragma GCC diagnostic pop

      // Performing a memset() on servaddr is quite important when doing
      //   socket communication.
      memset(&servaddr, 0, sizeof(servaddr));
      // Create a socket from the AF_INET family, that is a stream socket
      servaddr.sin_family = AF_INET;
      // An IPv4 address
      // Host-TO-Network-Long. Listen on any interface/IP of the system.
      //?
      servaddr.sin_addr.s_addr = INADDR_ANY;
      // Host-TO-Network-Short, the default port from above.
      servaddr.sin_port = htons(ip_port);
      // bind the listenfd
      sock_err = socket(AF_INET, SOCK_STREAM, 0);
      IF_FAIL(sock_err, "FAILED TO CONNECT SOCKET")
      sockfd = sock_err;
      sockopt_err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                               &force, sizeof(force));
      IF_FAIL(sockopt_err, "FAILED TO SET SOCKET ATTACHED WITH FORCE (sockopt)")
      bind_err = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
      IF_FAIL(bind_err, "FAILED TO ATTACH SOCKET (bind)");
      // listen on the listenfd
      listen_err = listen(sockfd, LISTENQ);
      IF_FAIL(listen_err, "FAILED TO LISTEN..?");

      fprintf(stdout, "Hostname: %s\n", hostname);
      fprintf(stdout, "IP:       %s\n", IPbuffer);
      fprintf(stdout, "Port:     %d\n", ip_port);
      fprintf(stdout, "verbose     %d\n", is_verbose);
      fprintf(stdout, "usleep_time %d\n", usleep_time);
    }

    // create the input handler thread
    {
      pthread_attr_init(attr);
      pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
      pthread_create((&cmd_thread), attr, server_commands, NULL);
      // client length
      memset(tids_t, 0, sizeof(tids_t));
      memset(tids, 0, sizeof(tids));
      CLRVRB
      clilen = sizeof(cliaddr);
      // Accept connections on the listenfd.
      for (;;) {

        memset(&cliaddr, 0, sizeof(cliaddr));
        listenfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        IF_FAIL(listenfd, "ACCEPT FAIL?");
        // if (listenfd < 0)  AUTO BLOCKS!

        // loop forever accepting connections
        // You REALLY want to memset to all zeroes before you get bytes from
        // the socket.
        memset(buf, 0, sizeof(buf));
        // read a cmd_t structure from the socket.
        // if zro bytes are read, close the scoket
        n = read(listenfd, buf, MAXLINE);

        // process the command from the client
        // in the process_connection() is where I divy out the put/get/dir
        // threads
        // nothing was read, EOF
        // close the scoket
        if (n != 0) {
          if (is_verbose) {
            fprintf(stdout, "Connection from client: <%s>\n", buf);
          }
          ADD_VERB(verbose_ch, "\nHEARD COMMAND", 2);
          while (current_connections >= MAXCONNECT) {
            fprintf(stderr, "TOO MANY CONNECTIONS, SLOWING IT DOWN");
            sleep(1);
          }
          process_connection(listenfd, buf, n);
        } else {
          fprintf(stdout, "EOF found on client connection socket, "
                          "closing connection.\n");
          close(listenfd);
        }
      }
    }
    close(sockfd);
    printf("Closing listen socket\n");
  }
  // this could be pthread_exit, I guess...
  pthread_attr_destroy(attr);

  return (EXIT_SUCCESS);
}

//============[---OTHER---]===========//
void EMERGENCY_EXIT(void) {
  for (int i = 0; i < MAXCONNECT; ++i) {
    if (tids[i] == NULL)
      continue;
    pthread_cancel(*tids[i]);
    free(tids[i]);
    tids[i] = NULL;
  }
}

void process_connection(int listenfd, void *buf, int n [[maybe_unused]]) {
  // I have to allocate one of these for each thread that is created.
  // The thread is responsible for calling free on it.
  cmd_t *cmd = (cmd_t *)malloc(sizeof(cmd_t));
  int ret, thread_id;
  pthread_attr_t *attr = NULL;
  // pthread_t *tid = tids[(thread_id = current_connections)] = (pthread_t*)
  // malloc(sizeof(pthread_t));

  memcpy(cmd, buf, sizeof(cmd_t));
  ADD_VERB(verbose_ch, " WITH NAME ", 2);
  ADD_VERB(verbose_ch, cmd->cmd, 2);

  cmd->sock = listenfd;
  if (is_verbose) {
    fprintf(stderr, "Request from client: <%s> <%s>\n", cmd->cmd, cmd->name);
  }

  ret = pthread_attr_init(attr);
  IF_FAIL(ret, "PTHREAD ATTRIBUTE STRUCT FAILED");
  ret = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE);
  IF_FAIL(ret, "PTHREAD ATTRIBUTE STRUCT DETACHED FAILED");
  ret = pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM);
  IF_FAIL(ret, "PTHREAD ATTRIBUTE STRUCT SCOPE FAILED");
  // ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  // IF_FAIL(ret, "PTHREAD ATTRIBUTE STRUCT DETACHED FAILED");

  if (strcmp(cmd->cmd, CMD_GET) == 0) {
    // create thread to handle get file
    tids[thread_id = current_connections] =
        (pthread_t *)malloc(sizeof(pthread_t));
    tids_t[thread_id] = GET;
    ret = pthread_create((tids[thread_id]), attr, thread_get, (void *)cmd);
    IF_FAIL(ret, "FAILED TO CREATE GET THREAD");
  } else if (strcmp(cmd->cmd, CMD_PUT) == 0) {
    // create thread to handle put file
    tids[thread_id = current_connections] =
        (pthread_t *)malloc(sizeof(pthread_t));
    tids_t[thread_id] = PUT;
    ret = pthread_create(tids[thread_id], attr, thread_put, (void *)cmd);
    IF_FAIL(ret, "FAILED TO CREATE PUT THREAD");
  } else if (strcmp(cmd->cmd, CMD_DIR) == 0) {
    // create thread to handle dir
    tids[thread_id = current_connections] =
        (pthread_t *)malloc(sizeof(pthread_t));
    tids_t[thread_id] = DIR;
    ret = pthread_create(tids[thread_id], attr, thread_dir, (void *)cmd);
    IF_FAIL(ret, "FAILED TO CREATE DIR THREAD");
  } else {
    // This should never happen since the checks are made on
    // the client side.
    fprintf(stderr, "ERROR: unknown command >%s< %d\n", cmd->cmd, __LINE__);
    close(listenfd);
    pthread_attr_destroy(attr);
    return;
    // close the socket
  }
  pthread_join(*tids[thread_id], (void *)&ret);
  free(tids[thread_id]);
  tids[thread_id] = NULL;
  tids_t[thread_id] = 0;
  pthread_attr_destroy(attr);
  if (ret == EXIT_SUCCESS) {
    VERB_ADD(verbose_ch, "\nTHREAD SUCCESS!", 1);
    VERB_PRINT(verbose_ch, is_verbose);
    CLRVRB
  } else if (ret == EXIT_FAILURE) {
    VERB_PRINT(verbose_ch, 3);
    fprintf(stderr, "SOMETHING WENT WRONG WITH THREAD, EXITING!");
    exit(EXIT_FAILURE);
  }
}

void *server_commands(void *p [[maybe_unused]]) {
  pthread_t tid [[maybe_unused]] = pthread_self();
  char cmd[80] = {'\0'};
  char *ret_val = NULL;
  // pthread_detach(tid);
  //  detach the thread

  server_help();
  for (;;) {
    fputs(">> ", stdout);
    ret_val = fgets(cmd, sizeof(cmd), stdin);
    if (ret_val == NULL) {
      // end of input, a control-D was pressed.
      break;
    }
    // STOMP on the pesky new line
    // cmd[strlen(cmd) - 1] = '\0';

    if (strlen(cmd) == 0) {
      continue;
    } else if (strcmp(cmd, SERVER_CMD_EXIT) == 0) {
      // I really should do something better than this.
      break;
    } else if (strcmp(cmd, SERVER_CMD_COUNT) == 0) {
      printf("total connections   %lu\n", tcount);
      printf("current connections %u\n", current_connections_get());
      printf("verbose             %d\n", is_verbose);
      printf("usleep_time         %d\n", usleep_time);
    } else if (strcmp(cmd, SERVER_CMD_VPLUS) == 0) {
      is_verbose++;
      printf("verbose set to %d\n", is_verbose);
    } else if (strcmp(cmd, SERVER_CMD_VMINUS) == 0) {
      is_verbose--;
      if (is_verbose < 0) {
        is_verbose = 0;
      }
      printf("verbose set to %d\n", is_verbose);
    } else if (strcmp(cmd, SERVER_CMD_UPLUS) == 0) {
      usleep_time += USLEEP_INCREMENT;
      printf("usleep_time set to %d\n", usleep_time);
    } else if (strcmp(cmd, SERVER_CMD_UMINUS) == 0) {
      usleep_time -= USLEEP_INCREMENT;
      if (usleep_time < 0) {
        usleep_time = 0;
      }
      printf("usleep_time set to %d\n", usleep_time);
    } else if (strcmp(cmd, SERVER_CMD_HELP) == 0) {
      server_help();
    } else {
      printf("command not recognized >>%s<<\n", cmd);
    }
  }

  // This is really harsh. It terminates on all existing threads.
  // This would probably be better with a good exit hander
  exit(EXIT_SUCCESS);
}

void server_help(void) {
  printf("available commands are:\n");
  printf("\t%s : show the total connection count "
         "and number current connection\n",
         SERVER_CMD_COUNT);
  printf("\t%s    : increment the is_verbose flag (current %d)\n",
         SERVER_CMD_VPLUS, is_verbose);
  printf("\t%s    : decrement the is_verbose flag (current %d)\n",
         SERVER_CMD_VMINUS, is_verbose);

  printf("\t%s    : increment the usleep_time variable (by %d, currently %d)\n",
         SERVER_CMD_UPLUS, USLEEP_INCREMENT, usleep_time);
  printf("\t%s    : decrement the usleep_time variable (by %d, currently %d)\n",
         SERVER_CMD_UMINUS, USLEEP_INCREMENT, usleep_time);

  printf("\t%s  : exit the server process\n", SERVER_CMD_EXIT);
  printf("\t%s  : show this help\n", SERVER_CMD_HELP);
}

// get from server, so I need to send data to the client.
void *thread_get(void *p) {
  char buffer[MAXLINE];
  pthread_t tid = pthread_self();
  cmd_t *cmd = (cmd_t *)p;
  char hold[20];
  int fd = 0;
  ssize_t bytes_read, bytes_sent = 0;
  snprintf(hold, 20, "tid= %lu", tid);

  // current_connections_inc();
  current_connections_inc();

  ADD_VERB(verbose_ch, " THREAD ", 3);
  ADD_VERB(verbose_ch, hold, 3);
  if (is_verbose) {
    fprintf(stderr, "Sending %s to client\n", cmd->name);
  }
  // ope the file in cmd->name, read-only
  fd = open(cmd->name, O_RDONLY);
  if (fd < 0) {
    // barf
    // close things up, free() things up and leave
    // decrement

    do {
      memset(buffer, 0, MAXLINE);
      if (usleep_time > 0)
        usleep(usleep_time * USLEEP_INCREMENT);
      bytes_read = read(fd, buffer, MAXLINE);
      if (bytes_read < 0) {
        close(fd);
        close(cmd->sock);
        free(cmd);
        cmd = NULL;
        fprintf(stderr, "\nREADING FAILED!\n");
        current_connections_dec();
        pthread_exit((void *)EXIT_FAILURE);
      } else if (bytes_read > 0) {
        bytes_sent = send(cmd->sock, buffer, bytes_read, 0);
        if (bytes_sent != bytes_read) {
          fprintf(stderr,
                  "\nbytes sent (%ld) != bytes received (%ld)\nBytes not "
                  "received = ",
                  bytes_sent, bytes_read);
          for (int i = (bytes_read - bytes_sent); i < bytes_read; ++i) {
            fprintf(stderr, "%.8B | ch(%c) | int(%d) \t", buffer[i], buffer[i],
                    buffer[i]);
            if (i % 3 == 0)
              fprintf(stderr, "\n");
          }
        }
      } else {
        ADD_VERB(verbose_ch, " CLOSED GFILE ", 1);
        close(cmd->sock);
        close(fd);
        free(cmd);
        cmd = NULL;
      }
    } while (bytes_read != 0);
  }
  // in a while loop, read from the file and write to the socket
  // within the while loop, if sleep_flap > 0, usleep()

  // close file descriptor
  // close socket
  // free

  current_connections_dec();

  pthread_exit((void *)EXIT_SUCCESS);
}

void *thread_put(void *p) {
  char hold[20];
  pthread_t tid = pthread_self();
  char buffer[MAXLINE];
  cmd_t *cmd = (cmd_t *)p;
  int fd = 0;
  int error [[maybe_unused]] = errno; // supress warnings about errno.h
  ssize_t bytes_read, bytes_wrote = 0;

  current_connections_inc();

  if (is_verbose) {
    fprintf(stderr, "VERBOSE: Receiving %s from client\n", cmd->name);
  }
  // open the file in cmd->name as write-only
  // truncate it if it aready exists
  fd = open(cmd->name, O_WRONLY | O_TRUNC | O_CREAT,
            S_IRWXG | S_IRWXO | S_IRWXU);
  do {
    memset(buffer, 0, MAXLINE);
    if (usleep_time > 0)
      usleep(usleep_time * USLEEP_INCREMENT);
    bytes_read = read(cmd->sock, buffer, MAXLINE);
    if (bytes_read < 0) {
      ADD_VERB(verbose_ch, " : FAILED READ ON PUT SOCKET : TID =", 1);
      snprintf(hold, 20, "%ld\n", tid);
      ADD_VERB(verbose_ch, hold, 1);
      close(cmd->sock);
      close(fd);
      free(cmd);
      cmd = NULL;
      current_connections_dec();
      pthread_exit((void *)EXIT_FAILURE);
    } else if (bytes_read > 0) {
      if (is_verbose) {
        fprintf(
            stderr,
            "\nREAD %ld BYTES -> WITH STARTING VALUES %.8B | %.8B | %.8B "
            "(%c%c%c)\n\t -> WITH ENDING VALUES %.8B | %.8B | %.8B (%c%c%c)\n",
            bytes_read, buffer[0], buffer[1], buffer[2], buffer[0], buffer[1],
            buffer[2], buffer[bytes_read - 3], buffer[bytes_read - 2],
            buffer[bytes_read - 1], buffer[bytes_read - 3],
            buffer[bytes_read - 2], buffer[bytes_read - 1]);
      }
      bytes_wrote = write(fd, buffer, MAXLINE);
      if (bytes_read != bytes_wrote) {
        fprintf(stderr,
                "\nbytes wrote (%ld) != bytes received (%ld)\nBytes not "
                "received = ",
                bytes_wrote, bytes_read);
        for (int i = (bytes_read - bytes_wrote); i < bytes_read; ++i) {
          fprintf(stderr, "%.8B | ch(%c) | int(%d) \t", buffer[i], buffer[i],
                  buffer[i]);
          if (i % 3 == 0)
            fprintf(stderr, "\n");
        }
      }
    } else {
      close(fd);
      close(cmd->sock);
      free(cmd);
      cmd = NULL;
      ADD_VERB(verbose_ch, " CLOSED PFILE ", 1);
    }
  } while (bytes_read != 0);

  // in a while loop, read from the socket and write to the file
  // within the while loop, if sleep_flap > 0, usleep()

  // close file descriptor
  // close socket
  // free

  current_connections_dec();

  pthread_exit((void *)EXIT_SUCCESS);
}

void *thread_dir(void *p) {
  char *strm;
  char buffer[MAXLINE];
  cmd_t *cmd = (cmd_t *)p;
  FILE *fp = NULL;
  ssize_t bytes_sent = 0;
  current_connections_inc();

  fp = popen("ls -lFABhS --si --group-directories-first --color", "r");
  if (fp == NULL) {
    // barf
    // close, free, skedaddle
    // decrement

    close(cmd->sock);
    free(cmd);
    cmd = NULL;
    current_connections_dec();
    pthread_exit((void *)EXIT_FAILURE);
  }
  // in a while loop, read from fp, write to the socket
  // I used fgets() to get data and then pushed the string out with write()
  do {
    memset(buffer, 0, sizeof(buffer));
    if (usleep_time > 0)
      usleep(usleep_time * USLEEP_INCREMENT);
    strm = fgets(buffer, MAXLINE, fp);
    // bytes_read = fread(buffer, 1, MAXLINE, fp);
    if (strm == NULL) {
      if (is_verbose)
        fprintf(stderr, "EOF REACHED OR ERROR IDK");
      pclose(fp);
      close(cmd->sock);
      free(cmd);
      cmd = NULL;
      current_connections_dec();
      pthread_exit((void *)EXIT_FAILURE);
    } else if (strm != NULL) {
      bytes_sent = send(cmd->sock, buffer, strlen(buffer), 0);
      IF_FAIL(bytes_sent, "ERROR IN DIR THREAD SENDING BUFFER");
    }

  } while (strm != NULL);

  pclose(fp);
  close(cmd->sock);
  free(cmd);
  cmd = NULL;
  // pclose
  // close the socket
  // free

  current_connections_dec();

  pthread_exit((void *)EXIT_SUCCESS);
}

// I should REALLY put these fucntions and their related variables
// in a seperate source file.
void current_connections_inc(void) {
  // lock
  pthread_mutex_lock(&connections_mutex);
  // increment both values
  tcount += ++current_connections;
  // unlock
  pthread_mutex_unlock(&connections_mutex);
}

void current_connections_dec(void) {
  // lock
  pthread_mutex_lock(&connections_mutex);
  // decrement one value
  --current_connections;
  // unlock
  pthread_mutex_unlock(&connections_mutex);
}

unsigned int current_connections_get(void) { return current_connections; }
