#include "fonts.h"
#include "programs.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"
#include "system.h"
#include <string.h>

void notesInit() {
  // draw status bar
  ST7789_SetAddressWindow(0, 0, 480 - 1, 320 - 1);
  ST7789_Select();
  for (u8 i = 0; i < ST7789_HEIGHT / HOR_LEN; i++) {
    memset(disp_buf, 0x00, sizeof(disp_buf));

    u8 j = 0;
    if (i == 0) {
      drawToolbar();
      j = 29;
    }

    // draw gray horizontal paddings
    for (j; j < HOR_LEN; j++) {
      const int padWidth = 52;
      memset_u16(&disp_buf[j * 480 * 2], 0x6529, padWidth * 2);
      memset_u16(&disp_buf[j * 480 * 2 + (480 - 70) * 2], 0x6529, padWidth * 2);
    }

    ST7789_WriteData(disp_buf, sizeof(disp_buf));
  }

  // print("\n\n\nhello \x80\n");
  // clearPrintBuffer();
  // printFlush(Font_11x18);
}

extern PROGRAM NOTES = {"home", notesInit, 0};