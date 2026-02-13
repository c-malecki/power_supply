#include "app.h"
#include "chan.h"
#include "MCP4725.h"
#include "INA219.h"
#include <stdint.h>
#include <stdio.h>

PWR_Chan_t chan_3v3;
PWR_Chan_t chan_5v;
PWR_Chan_t chan_vdc;

APP_Status_t App_Init(APP_t *app, I2C_HandleTypeDef *i2c_handle)
{
    APP_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    app->i2c_handle = i2c_handle;

    PWR_Chan_Init(&chan_3v3, 3.3, NULL, 0);
    PWR_Chan_Init(&chan_5v, 5.0, NULL, 0);
    PWR_Chan_Init(&chan_vdc, 0.0, NULL, 0);

    app->pwr_chans[CHAN_3V3] = &chan_3v3;
    app->pwr_chans[CHAN_5V] = &chan_5v;
    app->pwr_chans[CHAN_VDC] = &chan_vdc;

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

    float base_voltage = 3.3;
    float increments[6] = { 0, 1.7, 2, 2, 1, 2 };

    PWR_Chan_Enable(app->pwr_chans[CHAN_VDC], true);

    for (int i = 0; i < 6; i++) {
        uint8_t err =
            PWR_Chan_Set(app->pwr_chans[CHAN_VDC], app->i2c_handle, base_voltage + increments[i]);
        if (err != HAL_OK) {
            status.err = err;
            return status;
        }

        HAL_Delay(200);

        err = PWR_Chan_Update(app->pwr_chans[CHAN_VDC], app->i2c_handle);
        if (err != HAL_OK) {
            status.err = err;
            return status;
        }

        float voltage = app->pwr_chans[CHAN_VDC]->cur_voltage;
        float current = app->pwr_chans[CHAN_VDC]->cur_current;
        float power = voltage * current;

        printf("Target V: %.4fV\n", app->pwr_chans[CHAN_VDC]->target_voltage);
        printf("V: %.4fV | I: %.4fmA | P: %.4fmW\n", voltage, current, power);
        if (i == 5) {
            printf("\n");
        }

        HAL_Delay(1000);
    }

    return status;
}
