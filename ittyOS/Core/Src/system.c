#include "system.h"
#include "programs.h"
#include "rust.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <string.h>

extern u8 heldSwitches[60] = {0};
extern u8 lastHeldSwitches[60] = {0};

/*
char maps for 'åäö' (i am indeed swedish)

å - \x80
ä - \x81
ö - \x82
Å - \x83
Ä - \x84
Ö - \x85
*/

// the character maps of keyboard matrix. null byte means 0
char charKeys[ROWS_AMT][COLS_AMT] = {
    "1234567890+\x00", "qwertyuiop\x80\00", "asdfghjkl\x82\x81'",
    "\x00zxcvbnm,.\x00-", "\x00\x00<\x00\x00 \x00\x00\x00\x00\x00\x00"};
char charKeysShift[ROWS_AMT][COLS_AMT] = {
    "!\"#$%&/()=?\x00", "QWERTYUIOP\x83^", "ASDFGHJKL\x85\x84*",
    "\x00ZXCVBNM;:\x00_", "\x00\x00>\x00\x00 \x00\x00\x00\x00\x00\x00"};
char charKeysAlt[ROWS_AMT][COLS_AMT] = {
    "\x00@\x00$\x00\x00{[]}\\", "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00~",
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    "\x00|\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"};

char getKeyAt(u8 row, u8 col) {
  u8 shift = heldSwitches[3 * COLS_AMT + 0];
  u8 alt = heldSwitches[4 * COLS_AMT + 3] || heldSwitches[4 * COLS_AMT + 8];
  if (shift && alt)
    return 0;
  if (shift)
    return charKeysShift[row][col];
  if (alt)
    return charKeysAlt[row][col];
  return charKeys[row][col];
}

#ifdef KEYBOARD_MATRIX
void readSwitches() {
  for (u8 c = 0; c < COLS_AMT; c++) {
    HAL_GPIO_WritePin(cols[c].bus, cols[c].pin, 1);
    for (u8 r = 0; r < ROWS_AMT; r++) {
      heldSwitches[r * COLS_AMT + c] =
          HAL_GPIO_ReadPin(rows[r].bus, rows[r].pin);
    }
    HAL_GPIO_WritePin(cols[c].bus, cols[c].pin, 0);
    // todo: maybe add delay here if interference.
  }
}
#endif
#ifndef KEYBOARD_MATRIX
u8 inputBufferI = 0;
u8 inputBufferILast = 0;
// u8 bufferedKeys[][2] = {{2, 1}, {2, 0}, {3, 1}, {11, 0}, {11, 1},
//                         {2, 1}, {2, 1}, {2, 1}, {1, 4}};
u8 bufferedKeys[][2] = {{6, 3}, {11, 1}};
void readSwitches() {
  if (inputBufferI >= sizeof(bufferedKeys) / 2) {
    u8 x = bufferedKeys[inputBufferILast][0];
    u8 y = bufferedKeys[inputBufferILast][1];
    heldSwitches[y * COLS_AMT + x] = 0;
    return;
  }
  u8 x = bufferedKeys[inputBufferILast][0];
  u8 y = bufferedKeys[inputBufferILast][1];
  heldSwitches[y * COLS_AMT + x] = 0;
  x = bufferedKeys[inputBufferI][0];
  y = bufferedKeys[inputBufferI][1];
  heldSwitches[y * COLS_AMT + x] = 1;
  inputBufferILast = inputBufferI;
  inputBufferI += 1;
}
#endif

PROGRAM *openProgram;
int home = 1;
PROGRAM *changedOpenProgram;
int changedProgram = 0;
int changedProgramHome = 0;

void launchProgram(PROGRAM *p) {
  changedOpenProgram = p;
  changedProgramHome = (p->name == HOME.name);
  changedProgram = 1;
}

void systemInit() {
  openProgram = programs[0];
  // openProgram = programs[2];
  // home = 0;
  HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED, !DEBUG_LED_ON_STATE);
  HAL_Delay(160);
  initSPI(LCD_SPI_SPEED);
  ST7789_Init();
  HAL_GPIO_WritePin(LED_PORT, LED, 1);
  openProgram->init();
}

void systemUpdate() {
  readSwitches();
  if (changedProgram) {
    home = changedProgramHome;
    changedProgram = 0;
    openProgram = changedOpenProgram;
    openProgram->init();
  }
  if (!home) {
    if (heldSwitches[4 * COLS_AMT + 1]) {
      // super key pressed
      openProgram = &HOME;
      openProgram->init();
    }
  }
  openProgram->update();
  // readSwitches();
  // getKeyAt(1, 1);
  memcpy(lastHeldSwitches, heldSwitches, sizeof(heldSwitches));
  HAL_Delay(16);
}