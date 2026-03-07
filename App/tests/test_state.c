#include <stdio.h>
//
#include "app.h"
#include "power_controller.h"
#include "tests.h"

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