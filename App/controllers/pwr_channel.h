#ifndef __POWER_CHANNEL_H__
#define __POWER_CHANNEL_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef enum {
  PWR_CHAN_3V3,
  PWR_CHAN_5V,
  PWR_CHAN_VDC,
  PWR_CHAN_COUNT
} PWR_Channel_t;

typedef struct {
  int pk;
  float pi;
  float t;
} PWR_PID_t;

typedef struct
{
  float target_voltage;
  GPIO_TypeDef *mosfet_port;
  uint16_t mosfet_pin;
  float cur_voltage;
  float cur_current;
  bool output_enabled;
  PWR_PID_t *pid;
} PWR_Chan_t;

PWR_Chan_t *PWR_Chan_Init(float target_voltage, GPIO_TypeDef *mosfet_port, uint16_t mosfet_pin);

#endif