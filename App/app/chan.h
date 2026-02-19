#ifndef __PWR_CHAN_H__
#define __PWR_CHAN_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define MIN_VOLTAGE 0.0f
#define MAX_VOLTAGE 12.0f
#define MAX_CURRENT 1.0f

typedef enum {
    CHAN_3V3 = 0,
    CHAN_5V,
    CHAN_VDC
} PWR_Channels_t;

typedef struct
{
    float p_gain;
    float i_gain;
    float acc_err;
    float prev_error;
    uint32_t last_time;
} PWR_PID_t;

typedef struct
{
    float target_voltage;
    uint16_t dac_steps;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    float cur_voltage;
    float cur_current;
    float cur_power;
    bool output_enabled;
    PWR_PID_t *pid;
} PWR_Chan_t;

void PWR_Chan_Init(PWR_Chan_t *chan, float target_voltage, GPIO_TypeDef *mosfet_port,
                   uint16_t mosfet_pin);
void PWR_Chan_Enable(PWR_Chan_t *chan, bool enabled);
uint8_t PWR_Chan_Set(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle, float target_voltage);

#endif