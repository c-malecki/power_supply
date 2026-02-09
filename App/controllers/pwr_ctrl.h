#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "pwr_channel.h"

typedef struct
{
  I2C_HandleTypeDef *i2c_handle;
  PWR_Chan_t *channels[PWR_CHAN_COUNT];
} PWR_Ctrl_t;

typedef enum {
  PWR_OK = 0,
  PWR_ERROR_I2C,
  PWR_ERROR_OVERCURRENT,
  PWR_ERROR_OVERVOLTAGE
} PWR_Status_t;

PWR_Status_t PWR_Init(PWR_Ctrl_t *ctrl, I2C_HandleTypeDef *i2c);

PWR_Status_t PWR_Enable(PWR_Ctrl_t *ctrl, PWR_Channel_t chan, bool enable);

PWR_Status_t PWR_Set(PWR_Ctrl_t *ctrl, PWR_Channel_t chan, float target);

PWR_Status_t PWR_Read(PWR_Ctrl_t *ctrl, PWR_Channel_t chan);

void PWR_SweepRange(PWR_Ctrl_t *ctrl, uint8_t *count);

#endif