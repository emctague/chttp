#include <chttp/Logger.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

int L_will_log(Verbosity logger, int verbosity) {
  return logger >= verbosity;
}

void L_log(Verbosity logger, int verbosity, const char *fmt, ...) {
  if (logger >= verbosity) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

