#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "mcp4725.h"
#include "ina219.h"
#include "common.h"

#define VOLTAGE_VARIABLE_MIN_WHOLE 6
#define VOLTAGE_VARIABLE_MIN_DECIMAL 0

#define VOLTAGE_VARIABLE_MAX_WHOLE 15
#define VOLTAGE_VARIABLE_MAX_DECIMAL 0

#define VARIABLE_VOLTAGE_TOLLERANCE 0.05f

#define INA_READ_INTERVAL 350

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    PWR_BUCK_MAIN = 0,
    PWR_BUCK_6V5,
    PWR_BUCK_12V,
    PWR_BUCK_VARV,
    PWR_BUCK_COUNT
} Pwr_Buck;

typedef enum {
    PWR_CHAN_3V3 = 0,
    PWR_CHAN_5V,
    PWR_CHAN_VARV,
    PWR_CHAN_COUNT,
} Pwr_Chan;

typedef struct
{
    bool on;
    uint16_t gpio_pin;
    GPIO_TypeDef *gpio_port;
    INA_t ina;
} Pwr_Buck_t;

typedef struct
{
    bool toggle_pending;
    bool output_on;
    uint16_t mosfet_pin;
    GPIO_TypeDef *mosfet_port;
} Pwr_Chan_t;

typedef struct
{
    float pid_p_gain;
    float pid_i_gain;
    float pid_acc_error;
    float pid_prev_error;
    int32_t target_voltage_w;
    uint32_t target_voltage_d;
    uint32_t pid_last_time;
    MCP_t mcp;
} Pwr_Var_Control_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Pwr_Buck_t bucks[4];
    Pwr_Chan_t channels[3];
    Pwr_Var_Control_t var_ctrls;
    Error_Callback_t error_cb;
    void *error_ctx;
} Pwr_Ctrl_t;

void Pwr_Ctrl_Init(Pwr_Ctrl_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Pwr_Ctrl_Ping(Pwr_Ctrl_t *ctrl);
void Pwr_Ctrl_Run(Pwr_Ctrl_t *ctrl);

void Pwr_Buck_Toggle(Pwr_Ctrl_t *ctrl, Pwr_Buck buck);
void Pwr_Chan_Toggle(Pwr_Ctrl_t *ctrl, Pwr_Chan chan);
void Pwr_Chan_SetVarV(Pwr_Ctrl_t *ctrl, int32_t target_voltage_w, uint32_t target_voltage_d);

#endif // __POWER_CONTROLLER_H__