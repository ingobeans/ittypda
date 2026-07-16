#define LCD_CS GPIO_PIN_13
#define LCD_CS_PORT GPIOC
#define LCD_RST GPIO_PIN_14
#define LCD_RST_PORT GPIOC
#define LCD_RS GPIO_PIN_15
#define LCD_RS_PORT GPIOC

#define LED GPIO_PIN_0
#define LED_PORT GPIOC

#define DEBUG_LED GPIO_PIN_15
#define DEBUG_LED_PORT GPIOA

// technically out of spec i think but my sd card allows it :>
#define SD_MOUNT_SPI_SPEED SPI_BAUDRATEPRESCALER_4
#define SD_SPI_SPEED SPI_BAUDRATEPRESCALER_4
#define LCD_SPI_SPEED SPI_BAUDRATEPRESCALER_2

// #define MOSI GPIO_PIN_12
// #define MOSI_PORT GPIOC
// #define MISO GPIO_PIN_11
// #define MISO_PORT GPIOC
// #define SCK GPIO_PIN_10
// #define SCK_PORT GPIOC

#define SD_CS GPIO_PIN_1
#define SD_CS_PORT GPIOC
#define DEBUG_LED_ON_STATE 1

#define HSPI hspi3

#define KEYBOARD_MATRIX
#include "main.h"
#include "rust.h"
#define COLS_AMT 12
#define ROWS_AMT 5

typedef struct {
  u16 pin;
  GPIO_TypeDef *bus;
} SPECIFIC_PIN;

SPECIFIC_PIN cols[] = {{GPIO_PIN_0, GPIOA}, {GPIO_PIN_1, GPIOA},
                       {GPIO_PIN_2, GPIOA}, {GPIO_PIN_3, GPIOA},
                       {GPIO_PIN_4, GPIOA}, {GPIO_PIN_5, GPIOA},
                       {GPIO_PIN_6, GPIOA}, {GPIO_PIN_7, GPIOA},
                       {GPIO_PIN_4, GPIOC}, {GPIO_PIN_5, GPIOC}};

SPECIFIC_PIN rows[] = {{GPIO_PIN_8, GPIOC},
                       {GPIO_PIN_7, GPIOC},
                       {GPIO_PIN_6, GPIOC},
                       {GPIO_PIN_15, GPIOB},
                       {GPIO_PIN_14, GPIOB}};