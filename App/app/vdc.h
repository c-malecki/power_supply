#ifndef __VDC_H__
#define __VDC_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define VDC_MIN_VOLTAGE 0.0f
#define VDC_MAX_VOLTAGE 12.0f
#define VDC_MAX_CURRENT 1.0f

typedef enum {
    VDC_ROTARY_MODE_OFF = 0,
    VDC_ROTARY_MODE_ADJUST,
    VDC_ROTARY_MODE_CONFIRM,
} VDC_Rotary_Modes_t;

typedef struct
{
    GPIO_TypeDef *clk_port;
    GPIO_TypeDef *dt_port;
    GPIO_TypeDef *sw_port;
    uint16_t clk_pin;
    uint16_t dt_pin;
    uint16_t sw_pin;
    uint8_t last_clk;
    int32_t position;
    bool pressed;
    VDC_Rotary_Modes_t mode;
} VDC_Rotary_t;

typedef struct
{
    float p_gain;
    float i_gain;
    float acc_err;
    float prev_error;
    uint32_t last_time;
} VDC_PID_t;

typedef struct
{
    float target_voltage;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    uint16_t cur_dac_steps;
    float cur_voltage;
    float cur_current;
    float cur_power;
    bool output_enabled;
    VDC_PID_t *pid;
    VDC_Rotary_t *rot;
} VDC_Channel_t;

void VDC_Channel_Init(VDC_Channel_t *chan);
void VDC_Channel_Enable(VDC_Channel_t *chan, bool enabled);
uint8_t VDC_Rotary_Poll(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle);
uint8_t VDC_Channel_Set(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle, float target_voltage);

#endif