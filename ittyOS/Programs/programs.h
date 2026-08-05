#pragma once

#include "home.h"
#include "notes.h"

#define COLS_AMT 12
#define ROWS_AMT 5

typedef struct {
  u64 *data;
  u16 height;
} Icon;

#define PROGRAMS_AMT 2
extern PROGRAM *programs[PROGRAMS_AMT];

void drawIcon(u16 x, i16 y, u16 color, u8 *buffer, u16 bufferWidth,
              u16 bufferHeight, Icon *icon);
void drawToolbar(char *name);
void updateToolbar();
