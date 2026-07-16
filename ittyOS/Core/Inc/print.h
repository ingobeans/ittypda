#pragma once
#define PRINT_INC

#include "rust.h"
#include <stdarg.h>
#include <stdio.h>

// print buffer
#define PRINT_BUFFER_MAX 2048
extern char printBuffer[PRINT_BUFFER_MAX];
extern u16 printLength;

void print(char *fmt, ...);
