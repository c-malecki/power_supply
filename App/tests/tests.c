#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "power_controller.h"
#include "led_controller.h"
#include "tests.h"
#include "MCP4725.h"
#include "INA219.h"
#include "stm32f4xx_hal_gpio.h"

void Test_LEDs(App_t *app)
{
    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_RED);
    LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_RED);
    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_RED);
    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_RED);
    HAL_Delay(1000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_GREEN);
    LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_GREEN);
    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_GREEN);
    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_GREEN);
    HAL_Delay(1000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_BLUE);
    LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_BLUE);
    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_BLUE);
    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_BLUE);
    HAL_Delay(1000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_YELLOW);
    LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_YELLOW);
    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_YELLOW);
    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_YELLOW);
    HAL_Delay(1000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_OFF);
    LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_OFF);
    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_OFF);
    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_OFF);
    HAL_Delay(1000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_GREEN);
}

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

void print_power_controller_state(Power_Controller_t *ctrl)
{
    Power_Controller_Channel_t chan3v3 = ctrl->channels[POWER_CHANNEL_3V3];
    Power_Controller_Channel_t chan5v = ctrl->channels[POWER_CHANNEL_5V];
    Power_Controller_Channel_t chanVar = ctrl->channels[POWER_CHANNEL_VARIABLE];

    printf("Power Controller:\n3V3 enabled: %u\n5V enabled: %u\nVARIABLE enabled: "
           "%u\ntarget_voltage: %.04f\ncur_voltage: %.04f\ncur_current: %.04f\ncur_power: "
           "%.04f\ncur_dac_steps: %u\r\n\n",
           chan3v3.output_enabled, chan5v.output_enabled, chanVar.output_enabled,
           chanVar.target_voltage, chanVar.variable.cur_voltage, chanVar.variable.cur_current,
           chanVar.variable.cur_power, chanVar.variable.cur_dac_steps);
}

void Test_PrintAppState(App_t *app)
{
    printf("APP STATE\n**********\r\n");
    // TODO: print_display_controller_state
    // TODO: print_input_controller_state
    print_power_controller_state(&app->pwr_ctrl);
    // TODO: print_temperature_controller_state
}