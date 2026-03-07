#include <stdbool.h>
#include <stdio.h>
//
#include "app.h"
#include "power_controller.h"
#include "tests.h"

void print_variable_channel(Power_Controller_Channel_t channel)
{
    printf("target_voltage: %.4fV\ncur_voltage: "
           "%.4fV\ncur_dac_steps: %u\ncur_current: %.4fmA\r\n\n",
           channel.target_voltage, channel.variable.cur_voltage, channel.variable.cur_dac_steps,
           channel.variable.cur_current);
}

void Test_VariableChannelLevels(App_t *app)
{
    printf("Test_VariableChannelLevels\n**********\r\n");

    Power_Controller_EnableChannel(&app->pwr_ctrl, POWER_CHANNEL_VARIABLE, true);

    // step up the voltage levels
    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_VARIABLE_MIN);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_3V3);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_5V);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_9V);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_10V);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_VARIABLE_MAX);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    // test moving from min to max to min
    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_VARIABLE_MIN);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_VARIABLE_MAX);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    app->status.err = Power_Controller_SetVariableVoltage(&app->pwr_ctrl, VOLTAGE_VARIABLE_MIN);
    App_Status_Check(app);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_EnableChannel(&app->pwr_ctrl, POWER_CHANNEL_VARIABLE, false);
    printf("channel enabled: %u\r\n",
           app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE].output_enabled);
    HAL_Delay(2000);
    print_variable_channel(app->pwr_ctrl.channels[POWER_CHANNEL_VARIABLE]);
}