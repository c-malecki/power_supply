#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f4xx_hal.h"

#define OLED_I2C_ADDR 0x3C << 1

#define OLED_DSP_OFF 0xAE
#define OLED_DSP_ON 0xAF
#define OLED_DSP_CONTRAST 0x81
#define OLED_DSP_RAM_NORMAL 0xA4
#define OLED_DSP_RAM_IGNORE 0xA5

uint8_t OLED_WriteCmd(I2C_HandleTypeDef *handle);

#endif