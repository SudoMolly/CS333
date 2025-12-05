#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "xorfuncs.c"


static bool diagnostics = false;


void show(int num, int file, const char *msg, const char *function, int line, ...) {
  int number_per_arg;
  va_list vargs;
  const char* hold = NULL;
  va_start(vargs, line);
  if (!diagnostics) return;
  perror("ERROR: ");
  dprintf(file,"%s\tfunction: %s\t line: %d\n", msg, function, line);
  for (int i = 0; i < num; ++i) {
    number_per_arg = va_arg(vargs, int);
    for (int j = 0; j < number_per_arg; ++j) {
      hold = va_arg(vargs,char*);
      dprintf(file, "%s", hold);
    }
    dprintf(file, "\n");
  }
  va_end(vargs);
  error_exit();
}
#define SHOW_MACRO(str,num) show(num, EOUT,str, __func__, __LINE__,

