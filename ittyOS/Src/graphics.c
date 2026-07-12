#include "graphics.h"
#include "fonts.h"
#include "main.h"
#include <stdarg.h>
#include <stdio.h>

#include "fatfs.h"
#include "ff.h"
#include "ffconf.h"
#include "st7789.h"

// use disp_buf for file streaming too
#define FILE_STREAM_BUF disp_buf
#define FILE_STREAM_BUF_SIZE sizeof(disp_buf)

// print buffer
#define PRINT_BUFFER_MAX 2048
char print_output[PRINT_BUFFER_MAX];
uint16_t print_length = 0;

// included in stdio.h but my LSP doesnt understand that
int vsnprintf(char *restrict buffer, size_t bufsz, const char *restrict format,
              va_list vlist);

void print(char *fmt, ...) {
  char *formatted_string = &print_output[print_length];
  if (print_length >= PRINT_BUFFER_MAX) {
    print_output[PRINT_BUFFER_MAX - 1] = '#';
    return;
  }
  va_list argptr;
  va_start(argptr, fmt);
  print_length += vsnprintf(formatted_string,
                            PRINT_BUFFER_MAX - print_length - 1, fmt, argptr);
  va_end(argptr);
}

void clear_print_buffer() { print_length = 0; }

void print_flush(FontDef font) {
  u16 line_height = font.height * 6 / 5;
  u16 offset = 0;
  char buf[128];

  if (print_length < PRINT_BUFFER_MAX - 1) {
    print_output[print_length + 1] = 0;
  }

  int p = 0;
  for (int i = 0; i < PRINT_BUFFER_MAX; i++) {
    char c = print_output[i];
    if (c == 0)
      break;

    if (c == '\r') {
    } else if (c == '\n' || p >= (480 / font.width)) {
      buf[p] = 0;
      p = 0;
      ST7789_WriteString(0, offset, buf, font, WHITE, BLACK);
      offset += line_height;
      if (offset >= 320 - 2 * line_height) {
        ST7789_WriteString(0, offset, "...continuing in 5 seconds", font, WHITE,
                           BLACK);
        HAL_Delay(5000);
        offset = 0;
        ST7789_Fill_Color(BLACK);
      }
      if (c != '\n') {
        buf[p] = c;
        p += 1;
      }

    } else {
      buf[p] = c;
      p += 1;
    }
  }
  buf[p] = 0;
  ST7789_WriteString(0, offset, buf, font, WHITE, BLACK);
}

void initSPI(u32 BaudRatePrescaler) {
  HSPI.Instance = SPI1;
  HSPI.Init.Mode = SPI_MODE_MASTER;
  HSPI.Init.Direction = SPI_DIRECTION_2LINES;
  HSPI.Init.DataSize = SPI_DATASIZE_8BIT;
  HSPI.Init.CLKPolarity = SPI_POLARITY_LOW;
  HSPI.Init.CLKPhase = SPI_PHASE_1EDGE;
  HSPI.Init.NSS = SPI_NSS_SOFT;
  HSPI.Init.BaudRatePrescaler = BaudRatePrescaler;
  HSPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
  HSPI.Init.TIMode = SPI_TIMODE_DISABLE;
  HSPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  HSPI.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&HSPI) != HAL_OK) {
    Error_Handler();
  }
}

typedef struct {
  u32 totalBytesRead;
  u16 chunkIndex;
} STREAM_FILE_CTX;

int streamFile(char *filename, void (*f)(STREAM_FILE_CTX)) {
  FIL file;

  FRESULT res = f_open(&file, filename, FA_READ);
  if (res != FR_OK) {
    print("f_open failed with code: %d\r\n", res);
    return res;
  }

  UINT bytes_read = 0;
  STREAM_FILE_CTX ctx = {12, 0};
  while (1) {
    res = f_lseek(&file, ctx.totalBytesRead);
    if (res != FR_OK) {
      print("f_seek failed with code: %d\r\n", res);
    }
    res = f_read(&file, FILE_STREAM_BUF, FILE_STREAM_BUF_SIZE, &bytes_read);
    if (res != FR_OK) {
      print("f_read failed with code: %d\r\n", res);
    }
    ctx.totalBytesRead += bytes_read;
    if (bytes_read != FILE_STREAM_BUF_SIZE) {
      // end of file reached
      break;
    }

    // use streamed data here!
    f(ctx);
    ctx.chunkIndex++;
  }
  res = f_close(&file);
  if (res != FR_OK) {
    print("f_close failed with code: %d\r\n", res);
    return res;
  }
  print("%d bytes read!", ctx.totalBytesRead);
}

void _drawIBICallback() {
  initSPI(SPI_BAUDRATEPRESCALER_2);
  ST7789_Select();
  ST7789_WriteData(FILE_STREAM_BUF, FILE_STREAM_BUF_SIZE);
  ST7789_UnSelect();
  initSPI(SPI_BAUDRATEPRESCALER_8);
}
int drawIBI(char *filename) {
  ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
  streamFile(filename, _drawIBICallback);
}

void writeCharToBuffer(u16 x, i16 y, char ch, FontDef font, u16 color,
                       u8 *buffer, u16 bufferWidth, u16 bufferHeight) {
  uint32_t b, j;
  u32 count = 0;
  if (y > 0) {
    count += bufferWidth * 2 * y;
  }
  i16 start = 0;
  if (y < 0) {
    start = -y;
  }

  for (i16 i = start; i < font.height; i++) {
    if (i + y >= bufferHeight) {
      break;
    }
    count += x * 2;

    unsigned char bc = ch;
    if (bc >= 128) {
      bc -= 1;
    }
    b = font.data[((bc - 32) * font.height + i)];
    for (j = 0; j < font.width; j++) {
      if ((b << j) & 0x8000) {
        buffer[count] = color >> 8;
        buffer[count + 1] = color & 0xFF;
      }
      count += 2;
    }
    // move count by remaining horizontal bytes
    count += (bufferWidth - font.width - x) * 2;
  }
}

char *textOverlayText;
FontDef *textOverlayFont;
u16 textOverlayX;
u16 textOverlayY;

void _drawIBITextOverlayCallback(STREAM_FILE_CTX ctx) {
  i16 currentChunkY = ctx.chunkIndex * FILE_STREAM_BUF_SIZE / 480 / 2;
  if ((currentChunkY + HOR_LEN >= textOverlayY &&
       currentChunkY + HOR_LEN <= textOverlayY + textOverlayFont->height) ||
      (currentChunkY >= textOverlayY &&
       currentChunkY <= textOverlayY + textOverlayFont->height)) {
    // draw text!!
    i16 y = textOverlayY;
    u16 x = textOverlayX;
    u16 i = 0;
    while (textOverlayText[i]) {
      if (x + textOverlayFont->width >= ST7789_WIDTH ||
          y + textOverlayFont->height >= ST7789_HEIGHT) {
        break;
      }
      writeCharToBuffer(x, y - currentChunkY, textOverlayText[i],
                        *textOverlayFont, WHITE, FILE_STREAM_BUF, 480,
                        FILE_STREAM_BUF_SIZE / 480 / 2);
      x += textOverlayFont->width;
      i++;
    }
  }
  initSPI(SPI_BAUDRATEPRESCALER_2);
  ST7789_Select();
  ST7789_WriteData(FILE_STREAM_BUF, FILE_STREAM_BUF_SIZE);
  ST7789_UnSelect();
  initSPI(SPI_BAUDRATEPRESCALER_8);
}
int drawIBITextOverlay(u16 x, u16 y, char *text, FontDef *font,
                       char *filename) {
  ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
  textOverlayText = text;
  textOverlayFont = font;
  textOverlayX = x;
  textOverlayY = y;
  streamFile(filename, _drawIBITextOverlayCallback);
}