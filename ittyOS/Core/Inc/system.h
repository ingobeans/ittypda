#pragma once
#define VERSION "0.3.11"

#include "conf.h"
#include "graphics.h" // IWYU pragma: keep
#include "main.h"
#include "print.h" // IWYU pragma: keep
#include "rust.h"
#include "sd_functions.h" // IWYU pragma: keep

void systemUpdate();
void systemInit();

typedef struct {
  char *name;
  void (*init)(void);
  void (*update)(void);
} PROGRAM;
