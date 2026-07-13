#define VERSION "0.3.7"

#include "conf.h"
#include "main.h"
#include "rust.h"

void update();
void init();

// technically out of spec i think but my sd card allows it :>
#define SD_MOUNT_SPI_SPEED SPI_BAUDRATEPRESCALER_4
#define SD_SPI_SPEED SPI_BAUDRATEPRESCALER_4
#define LCD_SPI_SPEED SPI_BAUDRATEPRESCALER_2