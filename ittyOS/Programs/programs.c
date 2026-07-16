#include "fonts.h"
#include "rust.h"
#include "st7789.h"
#include <string.h>

const u64 cat[] = {0x0000000000000000, 0x0000000000000000, 0x000c000001e00000,
                   0x000c000007c00000, 0x000a00000c800000, 0x0009000031800000,
                   0x0009800021000000, 0x0008000002000000, 0x0004000000000000,
                   0x0000000000000000, 0x0000180600000000, 0x0000380e00000000,
                   0x7fe0380e00fc0000, 0x0000000001800000, 0x0000000000000000,
                   0x1ff0400000000000, 0x6000218103ff0000, 0x0000238200000000,
                   0x0ff0324600000000, 0x38001c3801c00000, 0x60000c1000780000,
                   0x0000000000060000, 0x0000000000000000};
void writeCat(u16 x, i16 y, u16 color, u8 *buffer, u16 bufferWidth,
              u16 bufferHeight) {
  u64 b, j;
  u32 count = 0;
  if (y > 0) {
    count += bufferWidth * 2 * y;
  }
  i16 start = 0;
  if (y < 0) {
    start = -y;
  }

  for (i16 i = start; i < 23; i++) {
    if (i + y >= bufferHeight) {
      break;
    }
    count += x * 2;

    b = cat[i];
    for (j = 0; j < 64; j++) {
      if ((b << j) & 0x8000000000000000) {
        buffer[count] = color >> 8;
        buffer[count + 1] = color & 0xFF;
      }
      count += 2;
    }
    // move count by remaining horizontal bytes
    count += (bufferWidth - 64 - x) * 2;
  }
}

void drawToolbar(char *name) {
  writeCat(10, 1, WHITE, disp_buf, 480, HOR_LEN);
  memset(&disp_buf[27 * 2 * 480], 0xff, 480 * 2);
  memset(&disp_buf[28 * 2 * 480], 0xff, 480 * 2);

  for (u8 i = 0; i < 3; i++) {
    const int barWidth = 310;
    memset(&disp_buf[(i * 7 + 6) * 2 * 480 + 72 * 2], 0xff, barWidth * 2);
    memset(&disp_buf[(i * 7 + 6 + 1) * 2 * 480 + 72 * 2], 0xff, barWidth * 2);
  }
  writeStringToBuffer(480 - 90, 0, "12:56", Font_16x26, WHITE, disp_buf, 480,
                      HOR_LEN);
  u32 len = strlen(name);
  u16 width = len * Font_16x26.width;
  u16 textX = (480 - width) / 2;

  for (u8 i = 0; i < 27; i++) {
    u16 padW = width + 14;
    u16 padX = (480 - padW) / 2;
    memset(&disp_buf[i * 2 * 480 + padX * 2], 0, padW * 2);
  }
  writeStringToBuffer(textX, 0, name, Font_16x26, WHITE, disp_buf, 480,
                      HOR_LEN);
}