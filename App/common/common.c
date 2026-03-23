#include <stdint.h>
#include "common.h"
#include <stdio.h>
#include <stdbool.h>
#include "led_controller.h"

// TODO: issue #2: create better error system
const char *_Controller_Lookup[] = {
    "Not Specified",  "Display Controller", "Input Controller",
    "LED Controller", "Power Controller",   "Temperature Controller",
};

const char *_Peripheral_Lookup[] = {
    "Not Specified", "MCP4725", "INA219 Main", "INA219 Var", "GME12864-13", "SHT31",
};

const char *_Function_Lookup[] = { "Not Specified",    "INA Ping",         "MCP Ping",
                                   "GME Ping",         "SHT Ping",         "INA Init",
                                   "INA Read Voltage", "INA Read Current", "MCP Voltage to Steps",
                                   "GME Write Yellow", "GME Write Blue",   "GME Write Char",
                                   "SHT Init",         "SHT Read" };

const _Error_Blink_t _Error_Blink_Lookup[] = {
    {
        .code = ERROR_NONE,
        .blinks = 0,
    },
    {
        .code = ERROR_I2C_GENERIC_ERROR,
        .blinks = 2,
    },
    {
        .code = ERROR_I2C_BUSY,
        .blinks = 3,
    },
    {
        .code = ERROR_I2C_TIMEOUT,
        .blinks = 4,
    },
    {
        .code = ERROR_POWER_OVERCURRENT,
        .blinks = 3,
    },
    {
        .code = ERROR_POWER_OVERVOLTAGE,
        .blinks = 2,
    },
    {
        .code = ERROR_GME_INVALID_STR_LEN,
        .blinks = 5,
    },
    {
        .code = ERROR_GME_INVALID_CHAR,
        .blinks = 5,
    },
    {
        .code = ERROR_UNKNOWN,
        .blinks = 5,
    },
};

const char *_Error_Message_Lookup[] = {
    "OK",
    "I2C unspecified error",
    "I2C busy",
    "I2C timeout",
    "Power overcurrent",
    "Power overvoltage",
    "GME invalid string length",
    "GME invalid character",
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