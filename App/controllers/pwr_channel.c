#include "pwr_channel.h"

PWR_Chan_t *PWR_Chan_Init(float target_voltage, GPIO_TypeDef *mosfet_port, uint16_t mosfet_pin) {
  PWR_Chan_t chan;
  PWR_PID_t pid;

  chan.target_voltage = target_voltage;
  chan.mosfet_port = mosfet_port;
  chan.mosfet_pin = mosfet_pin;
  chan.output_enabled = false;
  chan->pid = &pid;

  return &ch;
}