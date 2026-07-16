#include "graphics.h"
#include "fonts.h"
#include "main.h"
#include "print.h"
#include "st7789.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

// use disp_buf for file streaming too
#define FILE_STREAM_BUF disp_buf
#define FILE_STREAM_BUF_SIZE sizeof(disp_buf)

void clearPrintBuffer() { printLength = 0; }

void printFlush(FontDef font) {
  u16 lineHeight = font.height * 6 / 5;
  u16 offset = 0;
  char buf[128];

  if (printLength < PRINT_BUFFER_MAX - 1) {
    printBuffer[printLength + 1] = 0;
  }

  int p = 0;
  for (int i = 0; i < PRINT_BUFFER_MAX; i++) {
    char c = printBuffer[i];
    if (c == 0)
      break;

    if (c == '\r') {
    } else if (c == '\n' || p >= (480 / font.width)) {
      buf[p] = 0;
      p = 0;
      ST7789_WriteString(0, offset, buf, font, WHITE, BLACK);
      offset += lineHeight;
      if (offset >= 320 - 2 * lineHeight) {
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

STREAM_FILE_CTX beginFileStream(char *filename) {
  STREAM_FILE_CTX ctx = {0};

  FRESULT res = f_open(&ctx.file, filename, FA_READ);
  if (res != FR_OK) {
    print("f_open failed with code: %d\r\n", res);
    ctx.success = res;
    return ctx;
  }
  return ctx;
}

int streamFile(STREAM_FILE_CTX *ctx, int f(STREAM_FILE_CTX *)) {
  FRESULT res;
  if (ctx->success) {
    print("streamFile started with failed STREAM_FILE_CTX: %d\n", ctx->success);
    return ctx->success;
  }
  if (!ctx->chunkSize) {
    ctx->chunkSize = FILE_STREAM_BUF_SIZE;
  }
  if (!ctx->subchunkCount) {
    ctx->subchunkCount = 1;
  }
  while (1) {
    res = f_lseek(&ctx->file, ctx->totalBytesRead + ctx->readStartOffset);
    if (res != FR_OK) {
      print("f_seek failed with code: %d\r\n", res);
    }
    res = f_read(&ctx->file,
                 &FILE_STREAM_BUF[ctx->subchunkIndex * ctx->chunkSize],
                 ctx->chunkSize, &ctx->bytesRead);
    ctx->totalBytesRead += ctx->bytesRead;
    if (res != FR_OK) {
      print("f_read failed with code: %d\r\n", res);
    }
    if (ctx->bytesRead == 0) {
      if (ctx->subchunkIndex > 0) {
        f(ctx);
      }
      break;
    }

    if (ctx->subchunkIndex >= ctx->subchunkCount - 1) {
      ctx->subchunkIndex = 0;
      // use streamed data here!
      if (f != 0 && f(ctx)) {
        break;
      };
      ctx->chunkIndex++;
    } else {
      ctx->subchunkIndex += 1;
    }
    ctx->readStartOffset += ctx->stepSize;

    if (ctx->bytesRead != ctx->chunkSize) {
      if (ctx->subchunkIndex < ctx->subchunkCount - 1) {
        f(ctx);
      }
      // end of file reached
      break;
    }
  }
  res = f_close(&ctx->file);
  if (res != FR_OK) {
    print("f_close failed with code: %d\r\n", res);
    return res;
  }
  return 0;
}

int mint(u32 a, u32 b) {
  if (b < a) {
    return b;
  } else {
    return a;
  }
}

DRAW_IBI_CONFIG DEFAULT_DRAW_IBI_CONFIG = {0};
DRAW_IBI_CONFIG *activeDrawIBIConfig;

u32 drawIBIRealWidth;
u32 drawIBIWidth;
u32 drawIBIHeight;

int _drawIBICallback(STREAM_FILE_CTX *ctx) {
  u32 targetBytes = drawIBIWidth * 2 * drawIBIHeight;
  u32 streamBytesAmt = mint(targetBytes - ctx->totalBytesRead +
                                ctx->bytesRead * ctx->subchunkCount,
                            ctx->bytesRead * ctx->subchunkCount);
  // run callback
  if (activeDrawIBIConfig->callback != 0) {
    activeDrawIBIConfig->callback(ctx);
  }
  initSPI(LCD_SPI_SPEED);
  ST7789_Select();
  ST7789_WriteData(FILE_STREAM_BUF, streamBytesAmt);
  ST7789_UnSelect();
  initSPI(SD_SPI_SPEED);
  // HAL_Delay(700);
  if (ctx->totalBytesRead >= drawIBIWidth * drawIBIHeight * 2) {
    return 1;
  }
  return 0;
}
int drawIBI(char *filename, u16 x, u16 y, DRAW_IBI_CONFIG drawIBIConfig) {
  activeDrawIBIConfig = &drawIBIConfig;
  STREAM_FILE_CTX ctx = beginFileStream(filename);
  char header[12];
  char magic[5] = {0};
  FRESULT res = f_read(&ctx.file, header, 12, 0);
  memcpy(magic, header, 4);
  memcpy(&drawIBIWidth, &header[4], 4);
  memcpy(&drawIBIHeight, &header[8], 4);
  print("magic: '%s' - width: %d - height: %d\n", magic, drawIBIWidth,
        drawIBIHeight);
  drawIBIRealWidth = drawIBIWidth;

  if (activeDrawIBIConfig->cropWidth) {
    drawIBIWidth = activeDrawIBIConfig->cropWidth;
  } else if (activeDrawIBIConfig->cropX) {
    drawIBIWidth = drawIBIWidth - activeDrawIBIConfig->cropX;
  }
  if (activeDrawIBIConfig->cropHeight) {
    drawIBIHeight = activeDrawIBIConfig->cropHeight;
  } else if (activeDrawIBIConfig->cropY) {
    drawIBIHeight = drawIBIHeight - activeDrawIBIConfig->cropY;
  }

  if (activeDrawIBIConfig->cropWidth || (activeDrawIBIConfig->cropX)) {
    // if x-axis is cropped, chunk size needs to be aligned to image width

    // find largest chunk size, less than FILE_STREAM_BUF_SIZE, aligned to image
    ctx.subchunkCount =
        mint(FILE_STREAM_BUF_SIZE / (drawIBIRealWidth * 2), drawIBIHeight);
    ctx.stepSize = (drawIBIRealWidth - drawIBIWidth) * 2;
    ctx.chunkSize = drawIBIWidth * 2;
  }

  if (strcmp(magic, "ibi!")) {
    print("error: bad magic\n");
    return 1;
  }
  ctx.readStartOffset =
      drawIBIRealWidth * 2 * drawIBIConfig.cropY + 12 + drawIBIConfig.cropX * 2;
  if (activeDrawIBIConfig->centered) {
    x -= drawIBIWidth / 2;
    y -= drawIBIHeight / 2;
  }
  ST7789_SetAddressWindow(x, y, x + drawIBIWidth - 1, y + drawIBIHeight - 1);
  if (res != FR_OK) {
    print("f_close failed with code: %d\r\n", res);
    return res;
  }
  streamFile(&ctx, _drawIBICallback);
  // print("stream %d bytes\nsubcount: %d\nchunkSize: %d\n", ctx.totalBytesRead,
  //       ctx.subchunkCount, ctx.chunkSize);
  return 0;
}

int _drawIBIFullscreenCallback() {
  initSPI(LCD_SPI_SPEED);
  ST7789_Select();
  ST7789_WriteData(FILE_STREAM_BUF, FILE_STREAM_BUF_SIZE);
  ST7789_UnSelect();
  initSPI(SD_SPI_SPEED);
  return 0;
}
int drawIBIFullscreen(char *filename) {
  ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
  STREAM_FILE_CTX ctx = beginFileStream(filename);
  ctx.readStartOffset = 12;
  streamFile(&ctx, _drawIBIFullscreenCallback);
  print("streamed %d bytes\n", ctx.totalBytesRead);
  return 0;
}

void writeCharToBuffer(u16 x, i16 y, char ch, FontDef font, u16 color,
                       u16 bgColor, u8 colorOverwrite, u8 *buffer,
                       u16 bufferWidth, u16 bufferHeight) {
  u32 b, j;
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
      } else if (colorOverwrite) {
        buffer[count] = bgColor >> 8;
        buffer[count + 1] = bgColor & 0xFF;
      }
      count += 2;
    }
    // move count by remaining horizontal bytes
    count += (bufferWidth - font.width - x) * 2;
  }
}

void writeStringToBuffer(u16 x, i16 y, char *str, FontDef font, u16 color,
                         u8 *buffer, u16 bufferWidth, u16 bufferHeight) {
  while (*str) {
    writeCharToBuffer(x, y, *str, font, color, 0, 0, buffer, bufferWidth,
                      bufferHeight);
    x += font.width;
    str++;
  }
}

void writeStringToBufferBG(u16 x, i16 y, char *str, FontDef font, u16 color,
                           u16 bgColor, u8 *buffer, u16 bufferWidth,
                           u16 bufferHeight) {
  while (*str) {
    writeCharToBuffer(x, y, *str, font, color, bgColor, 1, buffer, bufferWidth,
                      bufferHeight);
    x += font.width;
    str++;
  }
}

void memset_u16(char *buf, u16 value, u8 size) {
  for (int i = 0; i < size; i += 2) {
    memcpy(&buf[i], &value, 2);
  }
}

char *textOverlayText;
FontDef *textOverlayFont;
u16 textOverlayX;
u16 textOverlayY;

void _drawIBITextOverlayCallback(STREAM_FILE_CTX *ctx) {
  i16 currentChunkY =
      (ctx->totalBytesRead - ctx->chunkSize * ctx->subchunkCount) /
      drawIBIWidth / 2;
  i16 chunkHeight = ctx->chunkSize * ctx->subchunkCount / drawIBIWidth / 2;
  if ((currentChunkY + chunkHeight >= textOverlayY &&
       currentChunkY + chunkHeight <= textOverlayY + textOverlayFont->height) ||
      (currentChunkY >= textOverlayY &&
       currentChunkY <= textOverlayY + textOverlayFont->height)) {
    // draw text!!
    i16 y = textOverlayY;
    u16 x = textOverlayX;
    u16 i = 0;
    while (textOverlayText[i]) {
      if (x + textOverlayFont->width >= drawIBIWidth ||
          y + textOverlayFont->height >= drawIBIHeight) {
        break;
      }
      writeCharToBuffer(x, y - currentChunkY, textOverlayText[i],
                        *textOverlayFont, WHITE, 0, 0, FILE_STREAM_BUF,
                        drawIBIWidth, chunkHeight);
      x += textOverlayFont->width;
      i++;
    }
  }
}
int drawIBITextOverlay(char *filename, u16 x, u16 y, char *text, FontDef *font,
                       u16 textX, u16 textY, DRAW_IBI_CONFIG cfg) {
  textOverlayText = text;
  textOverlayFont = font;
  textOverlayX = textX;
  textOverlayY = textY;
  cfg.callback = _drawIBITextOverlayCallback;
  drawIBI(filename, x, y, cfg);
}