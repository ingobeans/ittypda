#include "print.h"

char printBuffer[PRINT_BUFFER_MAX] = {0};
u16 printLength = 0;

// included in stdio.h but my LSP doesnt understand that
int vsnprintf(char *restrict buffer, size_t bufsz, const char *restrict format,
              va_list vlist);

void print(char *fmt, ...) {
  if (printLength >= PRINT_BUFFER_MAX) {
    printBuffer[PRINT_BUFFER_MAX - 1] = '#';
    return;
  }
  va_list argptr;
  va_start(argptr, fmt);
  printLength += vsnprintf(&printBuffer[printLength],
                           PRINT_BUFFER_MAX - printLength - 1, fmt, argptr);
  va_end(argptr);
}