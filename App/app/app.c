#include "app.h"
#include "fdc.h"
#include "main.h"
#include "vdc.h"
#include "MCP4725.h"
#include "INA219.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>

FDC_Channel_t chan_3v3;
FDC_Channel_t chan_5v;
VDC_Channel_t chan_vdc;

APP_Status_t App_Init(APP_t *app, I2C_HandleTypeDef *i2c_handle)
{
    APP_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    app->i2c_handle = i2c_handle;

    FDC_Channel_Init(&chan_3v3, 3.3f, MOSFET_3V3_GPIO_Port, MOSFET_3V3_Pin);
    FDC_Channel_Init(&chan_5v, 5.0f, MOSFET_5V_GPIO_Port, MOSFET_5V_Pin);
    VDC_Channel_Init(&chan_vdc);
    app->chan_vdc = &chan_vdc;

    HAL_StatusTypeDef err = INA_Init(i2c_handle);
    if (err != 0) {
        status.ctrl = PWR_CTRL;
        status.err = err;
        return status;
    }

    return status;
}

/**
* Test values were with ST-Link connected and 1kR as load
*
* V: 12.0280V | I: 11.9000mA | P: 143.1332mW
* V: 9.0240V | I: 8.9000mA | P: 80.3136mW
* V: 5.0640V | I: 5.0000mA | P: 25.3200mW
* V: 3.3080V | I: 3.1000mA | P: 10.2548mW

* V: 12.0280V | I: 11.9000mA | P: 143.1332mW
* V: 9.0200V | I: 8.9000mA | P: 80.2780mW
* V: 5.0560V | I: 4.6000mA | P: 23.2576mW
* V: 3.3040V | I: 3.2000mA | P: 10.5728mW

* V: 12.0280V | I: 11.9000mA | P: 143.1332mW
* V: 9.0160V | I: 9.2000mA | P: 82.9472mW
* V: 5.0520V | I: 4.7000mA | P: 23.7444mW
* V: 3.3080V | I: 3.4000mA | P: 11.2472mW
*
*/
APP_Status_t App_Test(APP_t *app)
{
    // TODO: add PI loop testing
    APP_Status_t status;
    status.ctrl = PWR_CTRL;

    float base_voltage = 0.0;
    float increments[7] = { 0.0, 3.3, 1.7, 2.0, 2.0, 1.0, 2.0 };

    VDC_Channel_Enable(app->chan_vdc, true);

    printf("***App_Test***\r\n\n");

    for (int i = 0; i < 7; i++) {
        base_voltage = base_voltage + increments[i];
        uint8_t err = VDC_Channel_Set(app->chan_vdc, app->i2c_handle, base_voltage);
        if (err != HAL_OK) {
            status.err = err;
            return status;
        }

        float voltage = app->chan_vdc->cur_voltage;
        float current = app->chan_vdc->cur_current;
        float power = voltage * current;

        printf("target_v: %.4f\ncur_v: %.4fV\ncur_i: %.4fmA\ncur_p: %.4fmW\r\n\n", base_voltage,
               voltage, current, power);

        HAL_Delay(2000);
    }

    return status;
}
