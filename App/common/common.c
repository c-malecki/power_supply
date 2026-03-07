#include <stdint.h>
#include "common.h"

const char *_Controller_Lookup[] = {
    "no controller specified", "display controller", "input controller",
    "led controller",          "power controller",   "temperature controller",
};

const char *_Peripheral_Lookup[] = {
    "no peripheral specified", "MCP4725", "INA219", "GME12864-13", "DS18B20",
};

const char *_Error_Message_Lookup[] = {
    "OK",
    "I2C unspecified error",
    "I2C busy",
    "I2C timeout",
    "Power overcurrent",
    "Power overvoltage",
    "DS18 busy",
    "DS18 unspecified bus error",
    "DS18 reset failed",
    "DS18 invalid data length",
    "DS18 ROM ID error",
    "unknown error",
};

_Error_Codes ConvHALError(uint8_t hal_error)
{
    _Error_Codes code = ERROR_NONE;

    switch (hal_error) {

    case 0:
        break;
    case 1:
        code = ERROR_I2C_GENERIC_ERROR;
        break;
    case 2:
        code = ERROR_I2C_BUSY;
        break;
    case 3:
        code = ERROR_I2C_TIMEOUT;
        break;

    default:
        code = ERROR_UNKNOWN;
    }

    return code;
}