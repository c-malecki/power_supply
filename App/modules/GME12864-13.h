#ifndef __GME_H__
#define __GME_H__

#include "stm32f4xx_hal.h"

#define GME_I2C_ADDR 0x3C << 1

#define GME_DSP_OFF 0xAE
#define GME_DSP_ON 0xAF
#define GME_DSP_CONTRAST 0x81
#define GME_DSP_RAM_NORMAL 0xA4
#define GME_DSP_RAM_IGNORE 0xA5

uint8_t GME_WriteCmd(I2C_HandleTypeDef *handle);

#endif