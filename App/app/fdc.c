#include "fdc.h"

void FDC_Channel_Init(FDC_Channel_t *chan, float target_voltage, GPIO_TypeDef *mosfet_port,
                      uint16_t mosfet_pin)
{
    chan->target_voltage = target_voltage;
    chan->mosfet_port = mosfet_port;
    chan->mosfet_pin = mosfet_pin;
    chan->output_enabled = false;
}