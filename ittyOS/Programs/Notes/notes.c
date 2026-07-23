#include "fonts.h"
#include "programs.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "system.h"
#include <string.h>

const char notesName[6] = "Notes";

void notesInit() {
  // draw status bar
  ST7789_SetAddressWindow(0, 0, 480 - 1, 320 - 1);
  ST7789_Select();
  for (u8 i = 0; i < ST7789_HEIGHT / HOR_LEN; i++) {
    memset(disp_buf, 0x00, sizeof(disp_buf));

    u8 j = 0;
    if (i == 0) {
      drawToolbar(notesName);
      j = 29;
    }

    // draw gray horizontal margins
    for (j; j < HOR_LEN; j++) {
      const int marginWidth = 52;
      memset_u16(&disp_buf[j * 480 * 2], 0x6529, marginWidth * 2);
      memset_u16(&disp_buf[j * 480 * 2 + (480 - marginWidth) * 2], 0x6529,
                 marginWidth * 2);
    }

    ST7789_WriteData(disp_buf, sizeof(disp_buf));
  }

  // print("\n\n\nhello \x80\n");
  // clearPrintBuffer();
  // printFlush(Font_11x18);
}

u16 adcValue = 0;
u16 batteryVoltageTimes10000 = 0;
u16 batteryPercentage = 0;
void notesUpdate() {
  // dont ask why the notes app displays battery charge...
  // only for testing purposes for the moment

  clearPrintBuffer();
  print("\n\n");
  // Start ADC Conversion
  HAL_ADC_Start(&hadc1);
  // Poll ADC1 Perihperal & TimeOut = 1mSec
  HAL_ADC_PollForConversion(&hadc1, 1);
  // Read The ADC Conversion Result & Map It To PWM DutyCycle
  if (adcValue) {
    adcValue *= 3;
    adcValue += HAL_ADC_GetValue(&hadc1);
    adcValue /= 4;
  } else {
    adcValue = HAL_ADC_GetValue(&hadc1);
  }
  batteryVoltageTimes10000 = adcValue * 33 / 4;
  print("adc reading: %d   \n", adcValue);

  int emulatedVoltage = batteryVoltageTimes10000 * 42000 / 33000;

  // follows the rough shape of this 3.7v lithium battery charge graph:
  // link:
  // https://canada1.discourse-cdn.com/flex028/uploads/core_electronics/original/2X/f/f6b21e6a7d5a0e3412f2182c67b25c2558cd663a.png
  if (emulatedVoltage > 38000) {
    batteryPercentage = (emulatedVoltage) / 100 - 320;
  } else if (emulatedVoltage > 37000) {
    batteryPercentage = (emulatedVoltage) * 4 / 100 - 1460;
  } else if (emulatedVoltage > 31000) {
    batteryPercentage = ((emulatedVoltage) * 33 / 100 - 10200) / 100;
  } else {
    batteryPercentage = 0;
  }
  // batteryPercentage = (emulatedVoltage - 31000) / (420 - 310);

  int wholePart = batteryVoltageTimes10000 / 10000;
  int modPart = batteryVoltageTimes10000 % 10000;
  print("voltage: %d.%04d   \n", wholePart, modPart);
  wholePart = emulatedVoltage / 10000;
  modPart = emulatedVoltage % 10000;
  print("voltage (emulated): %d.%04d   \n", wholePart, modPart);
  print("percentage: %d  \n", batteryPercentage);
  printFlush(Font_11x18);
}

extern PROGRAM NOTES = {notesName, notesInit, notesUpdate};