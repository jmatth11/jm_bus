#include "log.h"
#include <stdarg.h>
#include <stdio.h>

void info_log(const char *str, ...) {
  va_list argp;
  va_start(argp, str);
  vfprintf(stdout, str, argp);
  va_end(argp);
}

void debug_log(const char *str, ...) {
  va_list argp;
  va_start(argp, str);
  vfprintf(stdout, str, argp);
  va_end(argp);
}

void error_log(const char *str, ...) {
  va_list argp;
  va_start(argp, str);
  vfprintf(stderr, str, argp);
  va_end(argp);
};
