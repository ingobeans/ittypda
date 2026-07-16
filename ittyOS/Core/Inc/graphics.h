#pragma once
#define GRAPHICS

#include "ff.h"
#include "fonts.h"
#include "rust.h"

typedef struct {
  u8 success;
  u32 readStartOffset;
  u32 totalBytesRead;
  u32 bytesRead;
  u32 chunkSize;
  u16 chunkIndex;
  u16 stepSize;
  u16 subchunkIndex;
  u16 subchunkCount;
  FIL file;
} STREAM_FILE_CTX;

typedef struct {
  /// Configure crop start X. 0 counts as no cropping on X axis.
  u16 cropX;
  /// Configure crop start Y. 0 counts as no cropping on U axis.
  u16 cropY;
  /// Configure crop width. 0 counts as no width bounds.
  u16 cropWidth;
  /// Configure crop height. 0 counts as no height bounds.
  u16 cropHeight;
  /// Whether the image should be drawn centered around the position gived to
  /// the draw function
  u8 centered;
  /// Render callback
  void (*callback)(STREAM_FILE_CTX *);
} DRAW_IBI_CONFIG;

extern DRAW_IBI_CONFIG DEFAULT_DRAW_IBI_CONFIG;

void printFlush(FontDef font);
int drawIBIFullscreen(char *filename);
void initSPI(u32 BaudRatePrescaler);
void clearPrintBuffer();
int drawIBITextOverlay(char *filename, u16 x, u16 y, char *text, FontDef *font,
                       u16 textX, u16 textY, DRAW_IBI_CONFIG cfg);
int drawIBI(char *filename, u16 x, u16 y, DRAW_IBI_CONFIG drawIBIConfig);
void writeCharToBuffer(u16 x, i16 y, char ch, FontDef font, u16 color,
                       u16 bgColor, u8 colorOverwrite, u8 *buffer,
                       u16 bufferWidth, u16 bufferHeight);
void writeStringToBuffer(u16 x, i16 y, char *str, FontDef font, u16 color,
                         u8 *buffer, u16 bufferWidth, u16 bufferHeight);
void writeStringToBufferBG(u16 x, i16 y, char *str, FontDef font, u16 color,
                           u16 bgColor, u8 *buffer, u16 bufferWidth,
                           u16 bufferHeight);
void memset_u16(char *buf, u16 value, u8 size);