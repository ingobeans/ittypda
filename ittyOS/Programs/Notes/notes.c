#include "fonts.h"
#include "graphics.h"
#include "programs.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "system.h"
#include <string.h>

const char notesName[6] = "Notes";

const int marginWidth = 52;
void notesInit() {
  // draw status bar
  ST7789_SetAddressWindow(0, 0, 480 - 1, 320 - 1);
  ST7789_Select();
  for (u8 i = 0; i < ST7789_HEIGHT / HOR_LEN; i++) {
    memset(disp_buf, 0x00, sizeof(disp_buf));

    u8 j = 0;
    if (i == 0) {
      drawToolbar(notesName);
      j = 29;
    }

    // draw gray horizontal margins
    for (j; j < HOR_LEN; j++) {
      memset_u16(&disp_buf[j * 480 * 2], 0x6529, marginWidth * 2);
      memset_u16(&disp_buf[j * 480 * 2 + (480 - marginWidth) * 2], 0x6529,
                 marginWidth * 2);
    }

    ST7789_WriteData(disp_buf, sizeof(disp_buf));
  }

  // print("\n\n\nhello \x80\n");
  // clearPrintBuffer();
  // printFlush(Font_11x18);
}

char textBuffer[2048] = {};
u16 bufferPos = 0;
char keyIsHeld = 0;
char heldKeyX = 0;
char heldKeyY = 0;

void notesUpdate() {
  readSwitches();
  u16 oldBufferPos = bufferPos;
  for (int x = 0; x < COLS_AMT; x++) {
    for (int y = 0; y < ROWS_AMT; y++) {
      u8 value = heldSwitches[y * COLS_AMT + x];
      if (value != lastHeldSwitches[y * COLS_AMT + x]) {
        if (value) {
          heldKeyX = x;
          heldKeyY = y;
          keyIsHeld = 1;
        } else {
          if (heldKeyX == x && heldKeyY == y) {
            keyIsHeld = 0;
          }
        }
      }
    }
  }
  char heldKey = getKeyAt(heldKeyY, heldKeyX);
  if (heldKeyX == 11 && heldKeyY == 0) {
    // backspace
  }
  if (heldKey) {
    textBuffer[bufferPos] = heldKey;
    bufferPos += 1;
    if (bufferPos >= 2048) {
      bufferPos = 0;
    }
  }
  updateToolbar();
  FontDef font = Font_11x18;
  u16 writableAreaWidth = 480 - marginWidth * 2;
  u16 writableAreaWidthChars = writableAreaWidth / font.width;

  u16 w = font.width;
  u16 h = (font.height);
  if (oldBufferPos < bufferPos) {
    u16 x = marginWidth + bufferPos % writableAreaWidthChars * font.width;
    u16 y = 32 + bufferPos / writableAreaWidthChars * font.height;
    u16 w = font.width;
    u16 h = font.height;
    ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
    memset(disp_buf, 0, w * h * 2);
    writeCharToBuffer(0, 0, textBuffer[oldBufferPos], font, WHITE, BLACK, 1,
                      disp_buf, w, h);
    ST7789_WriteData(disp_buf, w * h * 2);
  }
}

extern PROGRAM NOTES = {notesName, notesInit, notesUpdate};