#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

typedef struct
{
    // cfg
    I2C_HandleTypeDef *i2c_handle;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    // state
    uint16_t mcp_target_voltage;
    double cur_voltage;
    double cur_current;
    bool output_enabled;
} Power_Controller_t;

typedef enum {
    PWR_OK = 0,
    PWR_ERROR_I2C,
    PWR_ERROR_OVERCURRENT,
    PWR_ERROR_OVERVOLTAGE
} Power_Status_t;

Power_Status_t Power_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c,
                          GPIO_TypeDef *mosfet_port, uint16_t mosfet_pin);
Power_Status_t Power_SetVoltage(Power_Controller_t *ctrl, uint16_t value);
Power_Status_t Power_Enable(Power_Controller_t *ctrl, bool enable);
Power_Status_t Power_Update(Power_Controller_t *ctrl);
Power_Status_t Power_Read(Power_Controller_t *ctrl, double *voltage, double *current);
void Power_SweepRange(Power_Controller_t *ctrl);

#endif