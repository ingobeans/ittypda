#define VERSION "0.3.1"

#include "main.h"

#define LCD_CS GPIO_PIN_13  // C
#define LCD_RST GPIO_PIN_14 // C
#define LCD_RS GPIO_PIN_15  // C

#define LED GPIO_PIN_0 // C

// #define MOSI GPIO_PIN_12 // C
// #define MISO GPIO_PIN_11 // C
// #define SCK GPIO_PIN_12 // B

#define SD_CS GPIO_PIN_1 // C

#define HSPI hspi3

void update();
void init();