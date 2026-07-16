#pragma once
#define VERSION "0.3.8"

#include "conf.h"
#include "graphics.h" // IWYU pragma: keep
#include "main.h"
#include "rust.h"
#include "sd_functions.h" // IWYU pragma: keep

void update();
void init();

typedef struct {
  char *name;
  void (*init)(void);
  void (*update)(void);
} PROGRAM;
