#include "pwr_channel.h"

void PWR_Chan_Init(PWR_Chan_t *channel, float target_voltage, GPIO_TypeDef *mosfet_port, uint16_t mosfet_pin) {

  channel.target_voltage = target_voltage;
  channel.mosfet_port = mosfet_port;
  channel.mosfet_pin = mosfet_pin;
  channel.output_enabled = false;
  channel->pid.kp = 300.0f;
  channel->pid.ki = 50.0f;
  channel->pid.integral = 0.0f;
  channel->pid.prev_error = 0.0f;
  channel->pid.last_time = HAL_GetTick();
}