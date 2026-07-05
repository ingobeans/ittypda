#include "main.h"

typedef struct {
  int pin;
  GPIO_TypeDef *bus;
} SPECIFIC_PIN;

SPECIFIC_PIN cols[] = {{GPIO_PIN_0, GPIOA}, {GPIO_PIN_1, GPIOA},
                       {GPIO_PIN_2, GPIOA}, {GPIO_PIN_3, GPIOA},
                       {GPIO_PIN_4, GPIOA}, {GPIO_PIN_5, GPIOA},
                       {GPIO_PIN_6, GPIOA}, {GPIO_PIN_7, GPIOA},
                       {GPIO_PIN_4, GPIOC}, {GPIO_PIN_5, GPIOC}};

SPECIFIC_PIN rows[] = {{GPIO_PIN_14, GPIOB},
                       {GPIO_PIN_15, GPIOB},
                       {GPIO_PIN_6, GPIOC},
                       {GPIO_PIN_7, GPIOC},
                       {GPIO_PIN_8, GPIOC}};

unsigned char colsAmt = sizeof(cols) / sizeof(cols[0]);
unsigned char rowsAmt = sizeof(rows) / sizeof(rows[0]);
unsigned char heldSwitches[60] = {0};

void readSwitches() {
  for (unsigned char c = 0; c < sizeof(cols) / sizeof(cols[0]); c++) {
    HAL_GPIO_WritePin(cols[c].bus, cols[c].pin, 1);
    for (unsigned char r = 0; r < sizeof(rows) / sizeof(rows[0]); r++) {
      heldSwitches[c * rowsAmt + r] =
          HAL_GPIO_ReadPin(rows[r].bus, rows[r].pin);
    }
    HAL_GPIO_WritePin(cols[c].bus, cols[c].pin, 0);
    // todo: maybe add delay here if interference.
  }
}

void update() {
  readSwitches();
  HAL_Delay(60);
}