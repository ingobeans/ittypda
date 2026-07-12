#include "graphics.h"
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

int streamFile(char *filename, void (*f)()) {
  FIL file;
  UINT total_bytes_read = 12;

  FRESULT res = f_open(&file, filename, FA_READ);
  if (res != FR_OK) {
    print("f_open failed with code: %d\r\n", res);
    return res;
  }

  UINT bytes_read = 0;
  while (1) {
    res = f_lseek(&file, total_bytes_read);
    if (res != FR_OK) {
      print("f_seek failed with code: %d\r\n", res);
    }
    res = f_read(&file, FILE_STREAM_BUF, FILE_STREAM_BUF_SIZE, &bytes_read);
    if (res != FR_OK) {
      print("f_read failed with code: %d\r\n", res);
    }
    total_bytes_read += bytes_read;
    if (bytes_read != FILE_STREAM_BUF_SIZE) {
      // end of file reached
      break;
    }

    // use streamed data here!
    f();
  }
  res = f_close(&file);
  if (res != FR_OK) {
    print("f_close failed with code: %d\r\n", res);
    return res;
  }
}

void _drawIbiImageCallback() {
  initSPI(SPI_BAUDRATEPRESCALER_2);
  ST7789_Select();
  ST7789_WriteData(FILE_STREAM_BUF, FILE_STREAM_BUF_SIZE);
  ST7789_UnSelect();
  initSPI(SPI_BAUDRATEPRESCALER_8);
}
int drawIbiImage(char *filename) {
  ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
  streamFile(filename, _drawIbiImageCallback);
}