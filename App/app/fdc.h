#ifndef __FDC_H__
#define __FDC_H__

#include "stm32f4xx_hal.h"
#include "vdc.h"
#include <stdint.h>

typedef struct
{
    float target_voltage;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    bool output_enabled;
} FDC_Channel_t;

void FDC_Channel_Init(FDC_Channel_t *chan, float target_voltage, GPIO_TypeDef *mosfet_port,
                      uint16_t mosfet_pin);

#endif