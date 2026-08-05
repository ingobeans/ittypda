#include "fonts.h"
#include "graphics.h"
#include "programs.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "system.h"
#include <string.h>

void catTest() {
  drawIBI("images/cat.ibi", 0, 0, DEFAULT_DRAW_IBI_CONFIG);
  DRAW_IBI_CONFIG cfg3 = {0, 0, 0, 104};
  drawIBI("images/cat.ibi", 480 - 260, 320 - 125, cfg3);
  DRAW_IBI_CONFIG cfg = {87, 0, 78, 0};
  drawIBI("images/cat.ibi", (480 - 250) / 2, (320 - 162) / 2 + 60, cfg);
  DRAW_IBI_CONFIG cfg2 = {87, 23, 78, 60, 1};
  drawIBI("images/cat.ibi", 480 / 2, 320 / 2, cfg2);
}
const u16 INPUT_BG = 0xD602;
const u16 TEXT_COLOR = 0xCE79;

u16 homeInputCursor = 0;
char homeInput[30] = {0};
char homeInputGuess[20] = {0};

int homeInputGuessCounter = 0;
PROGRAM *guessProgram() {
  for (int i = 0; i < PROGRAMS_AMT; i++) {
    PROGRAM *p = programs[i];
    homeInputGuessCounter = 0;
    while (1) {
      char a = p->name[homeInputGuessCounter];
      char b = homeInput[homeInputGuessCounter];
      if (b == 0) {
        return p;
      }
      if (a != b) {
        break;
      }
      homeInputGuessCounter++;
    }
  }
  return 0;
}

void homeDrawInputBox() {
  int w = 328;
  int h = 39;
  ST7789_SetAddressWindow(78, 231, 78 + w - 1, 231 + h - 1);
  // memset_u16(disp_buf, INPUT_BG, w * h * 2);
  memset_u16(disp_buf, INPUT_BG, w * h * 2);
  char *print = "Enter program or expression..";
  u16 color = TEXT_COLOR;
  homeInputGuess[0] = 0;
  if (homeInput[0]) {
    print = homeInput;
    color = 0xffff;

    // guess entry result
    PROGRAM *p = guessProgram();
    if (p) {
      memcpy(homeInputGuess, &p->name[homeInputGuessCounter],
             strlen(p->name) - homeInputGuessCounter);
    }
  }
  writeStringToBuffer(5, 5, print, Font_11x18, color, disp_buf, w, h);
  writeStringToBuffer(5 + strlen(print) * 11, 5, homeInputGuess, Font_11x18,
                      TEXT_COLOR, disp_buf, w, h);
  ST7789_WriteData(disp_buf, w * h * 2);
}

void homeInit() {
  initSPI(SD_MOUNT_SPI_SPEED);
  if (sd_mount() == FR_OK) {
    initSPI(SD_SPI_SPEED);
    drawIBI("images/wallpaper2.ibi", 0, 0, DEFAULT_DRAW_IBI_CONFIG);

    DRAW_IBI_CONFIG cfg = {0};
    cfg.cropX = 480 - 100;
    cfg.cropWidth = 100;
    cfg.cropHeight = 27;

    // catTest();
    HAL_Delay(600);
    sd_unmount();
  }
  initSPI(LCD_SPI_SPEED);
  homeDrawInputBox();

  // clearPrintBuffer();
  // ST7789_Fill_Color(BLACK);
  // printFlush(Font_7x10);
}

void homeUpdate() {
  for (int x = 0; x < COLS_AMT; x++) {
    for (int y = 0; y < ROWS_AMT; y++) {
      u8 value = heldSwitches[y * COLS_AMT + x];
      if (value) {
        char key = getKeyAt(y, x);
        if (key) {
          homeInput[homeInputCursor] = key;
          homeInputCursor++;
        } else if (x == 11 && y == 0) {
          // backspace
          homeInputCursor--;
          homeInput[homeInputCursor] = 0;
        }
      }
    }
  }
  homeDrawInputBox();
}

extern PROGRAM HOME = {"home", homeInit, homeUpdate};