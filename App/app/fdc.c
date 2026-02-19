#include "fdc.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

void FDC_Channel_Init(FDC_Channel_t *chan, float target_voltage, GPIO_TypeDef *mosfet_port,
                      uint16_t mosfet_pin)
{
    chan->target_voltage = target_voltage;
    chan->mosfet_port = mosfet_port;
    chan->mosfet_pin = mosfet_pin;
    chan->output_enabled = false;
}

void FDC_Channel_Enable(FDC_Channel_t *chan, bool enabled)
{
    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
    }
    chan->output_enabled = enabled;
}