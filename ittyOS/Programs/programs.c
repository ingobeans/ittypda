#include "programs.h"
#include "fonts.h"
#include "graphics.h"
#include "rust.h"
#include "st7789.h"
#include <string.h>

const u64 _cat[] = {0x0000000000000000, 0x0000000000000000, 0x000c000001e00000,
                    0x000c000007c00000, 0x000a00000c800000, 0x0009000031800000,
                    0x0009800021000000, 0x0008000002000000, 0x0004000000000000,
                    0x0000000000000000, 0x0000180600000000, 0x0000380e00000000,
                    0x7fe0380e00fc0000, 0x0000000001800000, 0x0000000000000000,
                    0x1ff0400000000000, 0x6000218103ff0000, 0x0000238200000000,
                    0x0ff0324600000000, 0x38001c3801c00000, 0x60000c1000780000,
                    0x0000000000060000, 0x0000000000000000};

Icon cat = {_cat, 23};

const u64 _charge[] = {
    0x3ffffffffc000000, 0x4000000002000000, 0x4000000001000000,
    0x4000000001000000, 0x4000000001000000, 0x4000000001c00000,
    0x4000000001c00000, 0x4000000001c00000, 0x4000000001c00000,
    0x4000000001c00000, 0x4000000001c00000, 0x4000000001c00000,
    0x4000000001c00000, 0x4000000001c00000, 0x4000000001000000,
    0x4000000001000000, 0x4000000001000000, 0x4000000002000000,
    0x3ffffffffc000000,
};
Icon charge = {_charge, 19};

void drawIcon(u16 x, i16 y, u16 color, u8 *buffer, u16 bufferWidth,
              u16 bufferHeight, Icon *icon) {
  u64 b, j;
  u32 count = 0;
  if (y > 0) {
    count += bufferWidth * 2 * y;
  }
  i16 start = 0;
  if (y < 0) {
    start = -y;
  }

  for (i16 i = start; i < icon->height; i++) {
    if (i + y >= bufferHeight) {
      break;
    }
    count += x * 2;

    b = icon->data[i];
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

u16 batteryX = 480 - 46;
u16 batteryY = 1;

u16 adcValue = 0;
u16 batteryVoltageTimes10000 = 0;
u16 batteryPercentage = 100;

void drawBatteryBars(int bufferWidth, int xOff, int yOff) {
  // return;
  u16 barFill = batteryPercentage * 48 / 100;
  u16 last = barFill / 8;
  u16 lastExtra = barFill % 8;

  for (int i = 0; i < last + 1; i++) {
    for (int j = 0; j < 15; j++) {
      int off = j;
      int l = 8;
      if (i == last) {
        l = lastExtra;
      }
      if (i == 0) {
        off = 0;
        if (j > l) {
          continue;
        }
        l -= j;
      } else if (i == 1) {
        if (off > 9) {
          if (l > off - 9) {
            l -= (off - 9);
          } else {
            l = 0;
          }
          off = 9;
        }
      } else if (i == 4) {
        if (l > off) {
          l = off;
        }
        if (l > 0) {
          l -= 1;
        }
      } else if (i == 5) {
        if (j < 10) {
          continue;
        }
        off += 1;
        if (l > off - 10) {
          l = off - 10;
        }
      }
      memset_u16(&disp_buf[(j + yOff) * bufferWidth * 2 + (xOff + i * 9) * 2 -
                           off * 2],
                 0xffff, l * 2);
    }
  }
}

void drawToolbar(char *name) {
  drawIcon(batteryX, batteryY, WHITE, disp_buf, 480, HOR_LEN, &charge);
  drawBatteryBars(480, 3 + batteryX, 2 + batteryY);
  memset(&disp_buf[27 * 2 * 480], 0xff, 480 * 2);
  memset(&disp_buf[28 * 2 * 480], 0xff, 480 * 2);

  for (u8 i = 0; i < 3; i++) {
    const int barWidth = 320;
    memset(&disp_buf[(i * 7 + 4) * 2 * 480 + 95 * 2], 0xff, barWidth * 2);
    memset(&disp_buf[(i * 7 + 4 + 1) * 2 * 480 + 95 * 2], 0xff, barWidth * 2);
  }
  writeStringToBuffer(5, 0, "12:56", Font_16x26, WHITE, disp_buf, 480, HOR_LEN);
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

// #define BATTERY_DEBUG

void updateToolbar() {
#ifdef BATTERY_DEBUG
  clearPrintBuffer();
  print("\n\n");
#endif

  int emulatedVoltage = 42000;

  // follows the rough shape of this 3.7v lithium battery charge graph:
  // link:
  // https://canada1.discourse-cdn.com/flex028/uploads/core_electronics/original/2X/f/f6b21e6a7d5a0e3412f2182c67b25c2558cd663a.png
  if (emulatedVoltage > 38000) {
    batteryPercentage = (emulatedVoltage) / 100 - 320;
  } else if (emulatedVoltage > 37000) {
    batteryPercentage = (emulatedVoltage) * 4 / 100 - 1460;
  } else if (emulatedVoltage > 33000) {
    batteryPercentage = ((emulatedVoltage)-33000) / 200;
  } else {
    batteryPercentage = 0;
  }

  ST7789_SetAddressWindow(batteryX, batteryY, batteryX + 42 - 1,
                          batteryY + 19 - 1);
  memset(disp_buf, 0, 42 * 19 * 2);
  ST7789_Select();
  drawIcon(0, 0, WHITE, disp_buf, 42, 19, &charge);
  drawBatteryBars(42, 3, 2);
  ST7789_WriteData(disp_buf, 42 * 19 * 2);

#ifdef BATTERY_DEBUG
  print("adc reading: %d   \n", adcValue);
  int wholePart = batteryVoltageTimes10000 / 10000;
  int modPart = batteryVoltageTimes10000 % 10000;
  print("voltage: %d.%04d   \n", wholePart, modPart);
  wholePart = emulatedVoltage / 10000;
  modPart = emulatedVoltage % 10000;
  print("voltage (emulated): %d.%04d   \n", wholePart, modPart);
  print("percentage: %d  \n", batteryPercentage);
  printFlush(Font_11x18);
#endif
}