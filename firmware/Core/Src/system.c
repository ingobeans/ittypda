#include "main.h"
#include "rust.h"

typedef struct {
  u16 pin;
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

u8 colsAmt = sizeof(cols) / sizeof(cols[0]);
u8 rowsAmt = sizeof(rows) / sizeof(rows[0]);
u8 heldSwitches[60] = {0};

void readSwitches() {
  for (u8 c = 0; c < colsAmt; c++) {
    HAL_GPIO_WritePin(cols[c].bus, cols[c].pin, 1);
    for (u8 r = 0; r < rowsAmt; r++) {
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