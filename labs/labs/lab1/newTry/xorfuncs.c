#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static bool verbose = false;

//If want to change names, only do so here
#define _GENERATE_ENUM(ENUM) ENUM,
#define _GENERATE_STRING(STRING) #STRING,

typedef enum error_list_e {
  _FOR_EACH_ERROR(_GENERATE_ENUM)
} _error_list;


typedef struct {
  bool cae;
  bool xor;
  bool e;
  bool d;
  char *path;
  char *out;
  char *c;
  char *x;
  int cLen;
  int xLen;
} OPTION;

static OPTION *option;
const int LANGSIZE = 94;
const int LANGLOW = 32;
const int LANGHIGH = 126;
const int LENGTHMAX = 10000;
void initialize() {
  if (option == NULL)
    option = (OPTION *)malloc(sizeof(OPTION));
  option->cae = option->xor = option->d = false;
  option->e = true;
  option->out = option->path = option->c = option->x = NULL;
  option->xLen = option->cLen = 0;
}

/*
void getoptions(char **argv, int argc) {
  int gOpt;
  size_t hold;
  char *some;
  int num;
  num = 0;
  if (option == NULL)
    initialize();
  while ((gOpt = getopt(argc, argv, "c:dex:")) != -1) {
    switch (gOpt) {
    case 'c':
      option->cae = true;
      option->c = strdup(optarg);
      option->cLen = strlen(option->c);
      break;
    case 'd' | 'e':
      option->d = !option->d;
      option->e = !option->e;
      break;
    case 'x':
      option->xor = true;
      option->x = strdup(optarg);
      option->xLen = strlen(option->x);
      break;
    default:
      // help
      break;
    }
  }
  while ((hold = read(, some, LENGTHMAX)) != -1) {
    num += hold;
  }
  option->path = (char *)malloc(num + 1);
  option->path = strncpy(option->path, argv[optind], num);
  option->path[num] = '\0';
  option->out = strdup(option->path);

  printf("%d", num);
}
*/


/*
char shiftright(int i) {
  char p = option->path[i];
  char k = option->c[i];
  p -= LANGLOW;
  k -= LANGLOW;
  return (((p + k) % (LANGSIZE + 1)) + LANGLOW);
}
*/

/*
char shiftleft(int i) {
  int path, key, diff;
  path = (int)option->path[i];
  key = (int)keyC(i);

  path -= LANGLOW;
  key -= LANGLOW;
  diff = path - key;

  return ((diff < 0 ? LANGSIZE + 1 - diff : diff) + LANGLOW);
}
*/


int main(int argc, char **argv) { int len;
  getoptions(argv, argc);
  if (option->path == NULL)
    // help
    len = strlen(option->path);

  if (option->e) {
    if (option->cae) {
      len = strlen(option->path);
      for (int i = 0; i < len; ++i)
        option->out[i] = shiftright(i);
    }
    if (option->xor)
      for (int i = 0; i < len; ++i)
        option->out[i] = xor(i);
  } else {
    if (option->xor)
      for (int i = 0; i < len; ++i)
        option->out[i] = xor(i);
    if (option->cae)
      for (int i = 0; i < len; ++i)
        option->out[i] = shiftleft(i);
  }
  write(STDOUT_FILENO, option->out, sizeof(char *) * (len));
  write(STDOUT_FILENO, "\n", 1);
}