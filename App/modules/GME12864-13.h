#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f4xx_hal.h"

#define OLED_I2C_ADDR 0x3C << 1

uint8_t OLED_WriteCmd(I2C_HandleTypeDef *handle);

#endif