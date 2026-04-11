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

void test_fixed_chans(Pwr_Ctrl_t *ctrl);
void test_var_chan(Pwr_Ctrl_t *ctrl);
void print_buck_ina(Pwr_Ctrl_t *ctrl, Pwr_Buck buck);

void Test_Power(App_t *app)
{
    printf("TEST POWER: begin\n**********\r\n\n");

    Pwr_Ctrl_t *ctrl = &app->power_controller;

    // check main power values
    INA_Read(&ctrl->bucks[PWR_BUCK_MAIN].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_MAIN);

    // toggle 3V3 and 5V channels and measure voltage/current change in main power
    test_fixed_chans(ctrl);

    // step up the voltage levels then toggle between min and max and measure changes in
    // voltage/current in both main power and variable out lines
    test_var_chan(ctrl);

    printf("TEST POWER: end\n**********\r\n\n");
}

// TODO: Needs PID loop because lower voltages are short of the target but
// the voltage is on target as it gets closer to 9V+
void test_var_chan(Pwr_Ctrl_t *ctrl)
{
    // variable channel is set to min during app init
    printf("Var channel out enabled\r\n");
    Pwr_Chan_Toggle(ctrl, PWR_CHAN_VARV);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    // test stepping up the voltage levels

    Pwr_Chan_SetVarV(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 7, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 8, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 9, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 10, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 11, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 12, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 13, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, 14, 0);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    Pwr_Chan_SetVarV(ctrl, VOLTAGE_VARIABLE_MAX_WHOLE, VOLTAGE_VARIABLE_MAX_DECIMAL);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    // test moving from min up to max voltage

    // min
    Pwr_Chan_SetVarV(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    // max
    Pwr_Chan_SetVarV(ctrl, VOLTAGE_VARIABLE_MAX_WHOLE, VOLTAGE_VARIABLE_MAX_DECIMAL);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    // min
    Pwr_Chan_SetVarV(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);

    HAL_Delay(2000);

    // disable again
    Pwr_Chan_Toggle(ctrl, PWR_CHAN_VARV);
    printf("Var channel out disabled\r\n");
    HAL_Delay(350);
    INA_Read(&ctrl->bucks[PWR_BUCK_VARV].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_VARV);
}

void test_fixed_chans(Pwr_Ctrl_t *ctrl)
{
    // 3V3 enabled and recheck main power values
    Pwr_Chan_Toggle(ctrl, PWR_CHAN_3V3);
    printf("3V3 channel out enabled\r\n");
    INA_Read(&ctrl->bucks[PWR_BUCK_6V5].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_6V5);
    HAL_Delay(2000);

    // 5V enabled and recheck main power values
    Pwr_Chan_Toggle(ctrl, PWR_CHAN_5V);
    printf("5V channel out enabled\r\n");
    INA_Read(&ctrl->bucks[PWR_BUCK_6V5].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_6V5);
    HAL_Delay(2000);

    // 3V3 disabled and recheck main power values
    printf("3V3 channel out disabled\r\n");
    Pwr_Chan_Toggle(ctrl, PWR_CHAN_3V3);
    INA_Read(&ctrl->bucks[PWR_BUCK_6V5].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_6V5);
    HAL_Delay(2000);

    // 5V disabled and recheck main power values
    printf("5V channel out disabled\r\n\n");
    Pwr_Chan_Toggle(ctrl, PWR_CHAN_5V);
    INA_Read(&ctrl->bucks[PWR_BUCK_6V5].ina, ctrl->i2c_handle);
    print_buck_ina(ctrl, PWR_BUCK_6V5);
    HAL_Delay(2000);
}

void print_buck_ina(Pwr_Ctrl_t *ctrl, Pwr_Buck buck)
{
    char *str = "";

    switch (buck) {

    case PWR_BUCK_MAIN:
        str = "BUCK MAIN";
        break;

    case PWR_BUCK_6V5:
        str = "BUCK 6V5";
        break;

    case PWR_BUCK_12V:
        str = "BUCK 12V";
        break;

    case PWR_BUCK_VARV:
        str = "BUCK VARV";
        break;

    default:
        break;
    }

    printf("%s:\nVoltage: %" PRId32 ".%03" PRIu32 "V\nCurrent: %" PRId32 ".%03" PRIu32 "mA\r\n\n",
           str, ctrl->bucks[PWR_BUCK_MAIN].ina.voltage_w, ctrl->bucks[PWR_BUCK_MAIN].ina.voltage_d,
           ctrl->bucks[PWR_BUCK_MAIN].ina.current_w, ctrl->bucks[PWR_BUCK_MAIN].ina.current_d);
}