#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "stm32f4xx_hal.h"
//
#include "app.h"
#include "power_controller.h"
#include "tests.h"
#include "common.h"

void Test_Power_VoltageCurrent(App_t *app)
{
    HAL_Delay(2000);

    printf("Test_Power_VoltageCurrent\n**********\r\n");

    Power_Controller_UpdateMainValues(&app->power_controller);
    printf("INA Main:\nVoltage: %" PRId32 ".%" PRIu32 "V\nCurrent: %" PRId32 ".%" PRIu32 "V\r\n\n",
           app->power_controller.main_voltage_whole, app->power_controller.main_voltage_decimal,
           app->power_controller.main_current_whole, app->power_controller.main_current_decimal);

    Power_Controller_UpdateVarValues(&app->power_controller);
    printf("INA Var:\nVoltage: %" PRId32 ".%" PRIu32 "V\nCurrent: %" PRId32 ".%" PRIu32 "V\r\n\n",
           app->power_controller.channels[POWER_CHANNEL_VARIABLE].variable.cur_voltage_whole,
           app->power_controller.channels[POWER_CHANNEL_VARIABLE].variable.cur_voltage_decimal,
           app->power_controller.channels[POWER_CHANNEL_VARIABLE].variable.cur_current_whole,
           app->power_controller.channels[POWER_CHANNEL_VARIABLE].variable.cur_current_decimal);
}

void print_variable_channel(Power_Controller_Channel_t channel)
{
    printf("target_voltage: %" PRId32 ".%" PRIu32 "V\n"
           "cur_voltage: %" PRId32 ".%" PRIu32 "V\n"
           "cur_current: %" PRId32 ".%" PRIu32 "mA\n"
           "cur_dac_steps: %u\r\n\n",
           channel.target_voltage_whole, channel.target_voltage_decimal,
           channel.variable.cur_voltage_whole, channel.variable.cur_voltage_decimal,
           channel.variable.cur_current_whole, channel.variable.cur_current_decimal,
           channel.variable.cur_dac_steps);
}

void Test_VariableChannelLevels(App_t *app)
{
    printf("Test_VariableChannelLevels\n**********\r\n");

    Power_Controller_EnableOut(&app->power_controller, POWER_CHANNEL_VARIABLE, true);

    // step up the voltage levels
    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_VARIABLE_MIN_WHOLE,
                                VOLTAGE_VARIABLE_MIN_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_3V3_WHOLE, VOLTAGE_3V3_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_5V_WHOLE, VOLTAGE_5V_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_9V_WHOLE, VOLTAGE_9V_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_10V_WHOLE, VOLTAGE_10V_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_VARIABLE_MAX_WHOLE,
                                VOLTAGE_VARIABLE_MAX_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    // test moving from min to max to min
    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_VARIABLE_MIN_WHOLE,
                                VOLTAGE_VARIABLE_MIN_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_VARIABLE_MAX_WHOLE,
                                VOLTAGE_VARIABLE_MAX_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_SetVoltage(&app->power_controller, VOLTAGE_VARIABLE_MIN_WHOLE,
                                VOLTAGE_VARIABLE_MIN_DECIMAL);
    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
    HAL_Delay(2000);

    Power_Controller_EnableOut(&app->power_controller, POWER_CHANNEL_VARIABLE, false);
    printf("channel enabled: %u\r\n",
           app->power_controller.channels[POWER_CHANNEL_VARIABLE].output_enabled);
    HAL_Delay(2000);

    print_variable_channel(app->power_controller.channels[POWER_CHANNEL_VARIABLE]);
}