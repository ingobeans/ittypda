#include "system.h"

void catTest() {
  drawIBI("images/cat.ibi", 0, 0, DEFAULT_DRAW_IBI_CONFIG);
  DRAW_IBI_CONFIG cfg3 = {0, 0, 0, 104};
  drawIBI("images/cat.ibi", 480 - 260, 320 - 125, cfg3);
  DRAW_IBI_CONFIG cfg = {87, 0, 78, 0};
  drawIBI("images/cat.ibi", (480 - 250) / 2, (320 - 162) / 2 + 60, cfg);
  DRAW_IBI_CONFIG cfg2 = {87, 23, 78, 60, 1};
  drawIBI("images/cat.ibi", 480 / 2, 320 / 2, cfg2);
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
    drawIBITextOverlay("images/wallpaper2.ibi", 480 - 100, 0, "12:45",
                       &Font_16x26, 0, 0, cfg);
    HAL_Delay(1000);
    drawIBITextOverlay("images/wallpaper2.ibi", 480 - 100, 0, "12:46",
                       &Font_16x26, 0, 0, cfg);
    HAL_Delay(1000);

    catTest();
    DRAW_IBI_CONFIG cfg2 = {87, 23, 78, 60, 1};
    drawIBI("images/cat.ibi", 480 / 2, 320 / 2, cfg2);
    HAL_Delay(600);
    sd_unmount();
  }
  initSPI(LCD_SPI_SPEED);

  // clearPrintBuffer();
  // ST7789_Fill_Color(BLACK);
  // printFlush(Font_7x10);
}

extern PROGRAM HOME = {"home", homeInit, 0};