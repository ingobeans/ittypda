#include "fonts.h"
#include "graphics.h"
#include "print.h"
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

#define TEXT_BUFFER_LENGTH 1024
char textBuffer[TEXT_BUFFER_LENGTH] = {};
u16 bufferPos = 0;
char keyIsHeld = 0;
char heldKeyX = 0;
char heldKeyY = 0;

u16 cursorX = 0;
u16 cursorY = 0;

#define NOTES_STATE_NEW_CHAR 1
#define NOTES_STATE_BACKSPACE 2
#define NOTES_STATE_NEW_LINE 3

void notesUpdate() {
  u8 state = 0;

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

  u16 oldCursorX = cursorX;
  u16 oldCursorY = cursorY;

  char heldKey = 0;
  if (keyIsHeld) {
    heldKey = getKeyAt(heldKeyY, heldKeyX);
    if (heldKey) {
      textBuffer[bufferPos] = heldKey;
      bufferPos += 1;
      cursorX += 1;
      state = NOTES_STATE_NEW_CHAR;
      if (bufferPos >= TEXT_BUFFER_LENGTH) {
        bufferPos = 0;
      }
    }

    if (heldKeyX == 11 && heldKeyY == 0) {
      // backspace
      bufferPos -= 1;
      if (cursorX == 0) {
        if (cursorY > 0) {
          cursorX = 0;
          cursorY -= 1;
        }
      } else {
        cursorX -= 1;
      }
      textBuffer[bufferPos] = 0;
      state = NOTES_STATE_BACKSPACE;
    } else if (heldKeyX == 11 && heldKeyY == 1) {
      // enter
      cursorY += 1;
      cursorX = 0;
      state = NOTES_STATE_NEW_LINE;
    }
  }
  updateToolbar();
  FontDef font = Font_11x18;
  u16 writableAreaWidth = 480 - marginWidth * 2;
  u16 writableAreaWidthChars = writableAreaWidth / font.width;

  u16 w = font.width;
  u16 h = (font.height);
  if (state == NOTES_STATE_NEW_CHAR) {
    u16 x = marginWidth + cursorX * font.width;
    u16 y = 32 + cursorY * font.height;
    u16 w = font.width;
    u16 h = font.height;
    ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
    memset(disp_buf, 0, w * h * 2);
    writeCharToBuffer(0, 0, textBuffer[oldBufferPos], font, WHITE, BLACK, 1,
                      disp_buf, w, h);
    ST7789_WriteData(disp_buf, w * h * 2);
  } else if (state == NOTES_STATE_BACKSPACE) {
    u16 x = marginWidth + oldCursorX * font.width;
    u16 y = 32 + oldCursorY * font.height;
    u16 w = font.width;
    u16 h = font.height;
    ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
    memset(disp_buf, 0, w * h * 2);
    ST7789_WriteData(disp_buf, w * h * 2);
  } else if (state == NOTES_STATE_NEW_LINE) {
  }

  // textBuffer[bufferPos] = ' ';
  // clearPrintBuffer();
  // print("\n\n\n                     ");
  // memcpy(&printBuffer[6], ' ', 10);
  // memcpy(&printBuffer[6], textBuffer, 10);
  // printFlush(Font_11x18);
}

extern PROGRAM NOTES = {notesName, notesInit, notesUpdate};