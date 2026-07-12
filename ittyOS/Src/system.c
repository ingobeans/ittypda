#include "system.h"
#include "fonts.h"
#include "graphics.h"
#include "rust.h"
#include "sd_functions.h"
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

void init() {
  HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED, !DEBUG_LED_ON_STATE);
  initSPI(SPI_BAUDRATEPRESCALER_2);
  ST7789_Init();
  HAL_GPIO_WritePin(LED_PORT, LED, 1);
  // print("\n\n\n\n");

  initSPI(SPI_BAUDRATEPRESCALER_256);
  if (sd_mount() == FR_OK) {
    initSPI(SPI_BAUDRATEPRESCALER_8);
    // drawIBIFullscreen("images/home.ibi");
    // drawIBITextOverlay(5, 65, "Hello there!!!", &Font_16x26,
    // "images/home.ibi");
    drawIBI("images/cat.ibi", 0, 0, DEFAULT_DRAW_IBI_CONFIG);
    DRAW_IBI_CONFIG cfg3 = {0, 0, 0, 104};
    drawIBI("images/cat.ibi", 480 - 260, 320 - 125, cfg3);
    DRAW_IBI_CONFIG cfg = {87, 0, 78, 0};
    clear_print_buffer();
    drawIBI("images/cat.ibi", (480 - 250) / 2, (320 - 162) / 2 + 60, cfg);
    DRAW_IBI_CONFIG cfg2 = {87, 23, 78, 60};
    drawIBI("images/cat.ibi", (480 - 250) / 2 + 110, (320 - 162) / 2 + 60,
            cfg2);
    sd_unmount();
  } else {
    initSPI(SPI_BAUDRATEPRESCALER_2);
  }

  // clear_print_buffer();
  // print("hello there");
  // ST7789_Fill_Color(BLACK);
  // print_flush(Font_11x18);
}

void update() {
  // readSwitches();
  // getKeyAt(1, 1);
  HAL_Delay(60);
}