#include "log.h"
#include <stdarg.h>
#include <stdio.h>


static void print_local(FILE* fp, const char* str, ...) {
  va_list argp;
  va_start(argp, str);
  fprintf(fp, str, argp);
  va_end(argp);
}

void info_log(const char *str, ...) {
  va_list argp;
  va_start(argp, str);
  print_local(stdout, str, argp);
  va_end(argp);
}

void debug_log(const char *str, ...) {
  va_list argp;
  va_start(argp, str);
  print_local(stdout, str, argp);
  va_end(argp);
}

void error_log(const char *str, ...) {
  va_list argp;
  va_start(argp, str);
  print_local(stderr, str, argp);
  va_end(argp);
};
