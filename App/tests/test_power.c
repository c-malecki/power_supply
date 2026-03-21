#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "stm32f4xx_hal.h"
//
#include "app.h"
#include "power_controller.h"
#include "tests.h"
#include "common.h"
#include "mcp4725.h"

void test_fixed_chans(Power_Controller_t *ctrl);
void test_var_chan(Power_Controller_t *ctrl);
void print_power_main(Power_Controller_t *ctrl);
void print_var_chan(Power_Controller_t *ctrl);

void Test_Power(App_t *app)
{
    printf("TEST POWER: begin\n**********\r\n\n");

    Power_Controller_t *ctrl = &app->power_controller;

    // check main power values
    Power_Controller_UpdateMainValues(ctrl);
    print_power_main(ctrl);

    // toggle 3V3 and 5V channels and measure voltage/current change in main power
    test_fixed_chans(ctrl);

    // step up the voltage levels then toggle between min and max and measure changes in
    // voltage/current in both main power and variable out lines
    test_var_chan(ctrl);

    printf("TEST POWER: end\n**********\r\n\n");
}

// TODO: Needs PID loop because lower voltages are short of the target but
// the voltage is on target as it gets closer to 9V+
// void test_var_chan(Power_Controller_t *ctrl)
// {
//     // variable channel is set to min during app init
//     printf("Var channel out enabled\r\n");
//     Power_Controller_ToggleOut(ctrl, POWER_CHANNEL_VARIABLE, true);
//     Power_Controller_UpdateVarValues(ctrl);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     // test stepping up the voltage levels

//     Power_Controller_SetVoltage(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 7, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 8, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 9, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 10, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 11, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 12, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 13, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, 14, 0);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     Power_Controller_SetVoltage(ctrl, VOLTAGE_VARIABLE_MAX_WHOLE, VOLTAGE_VARIABLE_MAX_DECIMAL);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     // test moving from min up to max voltage

//     // min
//     Power_Controller_SetVoltage(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     // max
//     Power_Controller_SetVoltage(ctrl, VOLTAGE_VARIABLE_MAX_WHOLE, VOLTAGE_VARIABLE_MAX_DECIMAL);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     // min
//     Power_Controller_SetVoltage(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
//     print_var_chan(ctrl);

//     HAL_Delay(2000);

//     // disable again
//     Power_Controller_ToggleOut(ctrl, POWER_CHANNEL_VARIABLE, false);
//     printf("Var channel out disabled\r\n");
//     print_var_chan(ctrl);
// }

// void test_fixed_chans(Power_Controller_t *ctrl)
// {
//     // 3V3 enabled and recheck main power values
//     Power_Controller_ToggleOut(ctrl, POWER_CHANNEL_3V3, true);
//     printf("3V3 channel out enabled\r\n");
//     Power_Controller_UpdateMainValues(ctrl);
//     HAL_Delay(2000);

//     // 5V enabled and recheck main power values
//     Power_Controller_ToggleOut(ctrl, POWER_CHANNEL_5V, true);
//     printf("5V channel out enabled\r\n");
//     Power_Controller_UpdateMainValues(ctrl);
//     HAL_Delay(2000);

//     // 3V3 disabled and recheck main power values
//     printf("3V3 channel out disabled\r\n");
//     Power_Controller_ToggleOut(ctrl, POWER_CHANNEL_3V3, false);
//     Power_Controller_UpdateMainValues(ctrl);
//     HAL_Delay(2000);

//     // 5V disabled and recheck main power values
//     printf("5V channel out disabled\r\n\n");
//     Power_Controller_ToggleOut(ctrl, POWER_CHANNEL_5V, false);
//     Power_Controller_UpdateMainValues(ctrl);
//     HAL_Delay(2000);
// }

void print_power_main(Power_Controller_t *ctrl)
{
    printf("MAIN POWER:\nVoltage: %" PRId32 ".%03" PRIu32 "V\nCurrent: %" PRId32 ".%03" PRIu32
           "mA\r\n\n",
           ctrl->main_voltage_whole, ctrl->main_voltage_decimal, ctrl->main_current_whole,
           ctrl->main_current_decimal);
}

void print_var_chan(Power_Controller_t *ctrl)
{
    Power_Controller_Channel_t vchan = ctrl->channels[POWER_CHANNEL_VARIABLE];

    printf("VARIABLE POWER:\nTarget: %" PRId32 ".%03" PRIu32 "V\n"
           "Voltage: %" PRId32 ".%03" PRIu32 "V\n"
           "Current: %" PRId32 ".%03" PRIu32 "mA\n"
           "DAC Steps: %u\r\n\n",
           vchan.target_voltage_whole, vchan.target_voltage_decimal,
           vchan.variable.cur_voltage_whole, vchan.variable.cur_voltage_decimal,
           vchan.variable.cur_current_whole, vchan.variable.cur_current_decimal,
           vchan.variable.cur_dac_steps);
}