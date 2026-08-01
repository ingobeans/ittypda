#pragma once
#define VERSION "0.3.14"

#include "conf.h"
#include "graphics.h" // IWYU pragma: keep
#include "main.h"
#include "print.h" // IWYU pragma: keep
#include "rust.h"
#include "sd_functions.h" // IWYU pragma: keep

void systemUpdate();
void systemInit();
char getKeyAt(u8 row, u8 col);
void readSwitches();

extern u8 heldSwitches[60];
extern u8 lastHeldSwitches[60];

typedef struct {
  char *name;
  void (*init)(void);
  void (*update)(void);
} PROGRAM;
