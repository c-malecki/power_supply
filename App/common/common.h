#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef enum {
    CONTROLLER_NONE = 0,
    CONTROLLER_DISPLAY,
    CONTROLLER_INPUT,
    CONTROLLER_LED,
    CONTROLLER_POWER,
    CONTROLLER_TEMPERATURE,
} _Controllers;

extern const char *_Controller_Lookup[];

typedef enum {
    PERIPHERAL_NONE = 0,
    PERIPHERAL_MCP,
    PERIPHERAL_INA,
    PERIPHERAL_GME,
    PERIPHERAL_DS18,
} _Peripherals;

extern const char *_Peripheral_Lookup[];

typedef enum {
    ERROR_NONE = 0,
    ERROR_I2C_GENERIC_ERROR,
    ERROR_I2C_BUSY,
    ERROR_I2C_TIMEOUT,
    ERROR_POWER_OVERCURRENT,
    ERROR_POWER_OVERVOLTAGE,
    ERROR_DS18_BUSY,
    ERROR_DS18_BUS,
    ERROR_DS18_RESET,
    ERROR_DS18_LEN,
    ERROR_DS18_ROM_ID,
    ERROR_GME_INVALID_STR_LEN,
    ERROR_GME_INVALID_CHAR,
    ERROR_UNKNOWN,
} _Error_Codes;

extern const char *_Error_Message_Lookup[];

_Error_Codes ConvHALError(uint8_t hal_error);

#endif // __COMMON_H__