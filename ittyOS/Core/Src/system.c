#include "system.h"
#include "programs.h"
#include "rust.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"

#ifdef KEYBOARD_MATRIX

u8 heldSwitches[60] = {0};

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

void systemInit() {
  HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED, !DEBUG_LED_ON_STATE);
  HAL_Delay(160);
  initSPI(LCD_SPI_SPEED);
  ST7789_Init();
  HAL_GPIO_WritePin(LED_PORT, LED, 1);
  HOME.init();
}

void systemUpdate() {
  // readSwitches();
  // getKeyAt(1, 1);
  HAL_Delay(60);
}