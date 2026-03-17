#include <stdio.h>
#include <inttypes.h>
//
#include "app.h"
#include "power_controller.h"
#include "temperature_controller.h"
#include "tests.h"

void print_power_controller_state(Power_Controller_t *ctrl)
{
    Power_Controller_Channel_t chan3v3 = ctrl->channels[POWER_CHANNEL_3V3];
    Power_Controller_Channel_t chan5v = ctrl->channels[POWER_CHANNEL_5V];
    Power_Controller_Channel_t chanVar = ctrl->channels[POWER_CHANNEL_VARIABLE];

    printf("Power Controller:\n"
           "3V3 enabled: %u\n"
           "5V enabled: %u\n"
           "VARIABLE enabled: %u\n"
           "target_voltage: %" PRId32 ".%03" PRIu32 "\n"
           "cur_voltage: %" PRId32 ".%03" PRIu32 "\n"
           "cur_current: %" PRId32 ".%03" PRIu32 "\n"
           "cur_dac_steps: %u\r\n\n",
           chan3v3.output_enabled, chan5v.output_enabled, chanVar.output_enabled,
           chanVar.target_voltage_whole, chanVar.target_voltage_decimal,
           chanVar.variable.cur_voltage_whole, chanVar.variable.cur_voltage_decimal,
           chanVar.variable.cur_current_whole, chanVar.variable.cur_current_decimal,
           chanVar.variable.cur_dac_steps);
}

void print_temperature_controller_state(Temperature_Controller_t *ctrl)
{
    printf("Temperature Controller:\nstate: %u\ncur_temp:%d\r\n\n", ctrl->state, ctrl->cur_temp);
}

void Test_PrintAppState(App_t *app)
{
    printf("APP STATE\n**********\r\n");
    // TODO: print_display_controller_state
    print_power_controller_state(&app->power_controller);
    // TODO: print_rotary_controller_state
    print_temperature_controller_state(&app->temperature_controller);
}