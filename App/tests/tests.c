#include "main.h"
#include "power_controller.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "tests.h"
#include "MCP4725.h"
#include "INA219.h"

App_Status_t Test_Channel_VAR(App_t *app)
{
    // TODO: add PI loop testing
    App_Status_t status;
    status.ctrl = PWR_CTRL;

    float base_voltage = 0.0;
    float increments[7] = { 0.0, 3.3, 1.7, 2.0, 2.0, 1.0, 2.0 };

    Channel_VAR_Enable_Output(app->pwr_ctrl->chan_var, true);

    printf("***Test_Channel_VAR***\r\n\n");

    for (int i = 0; i < 7; i++) {
        base_voltage = base_voltage + increments[i];
        uint8_t err =
            Channel_VAR_Set_Voltage(app->pwr_ctrl->chan_var, app->i2c_handle, base_voltage);
        if (err != HAL_OK) {
            status.err = err;
            return status;
        }

        float voltage = app->pwr_ctrl->chan_var->cur_voltage;
        float current = app->pwr_ctrl->chan_var->cur_current;
        float power = voltage * current;

        printf("target_v: %.4f\ncur_v: %.4fV\ncur_i: %.4fmA\ncur_p: %.4fmW\r\n\n", base_voltage,
               voltage, current, power);

        HAL_Delay(2000);
    }

    return status;
}
