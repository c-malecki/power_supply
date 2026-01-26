https://www.fischl.de/dcdccontrol/

https://www.handsontec.com/dataspecs/LM2596S-DC-DC.pdf

https://www.ti.com/lit/ds/symlink/lm2596.pdf

Vout = Vref * (1 + (R2 / R1)) where R1 = fixed resistor, R2 = potentiometer

With voltage injection where a resistor is added to FB -> DACout

Vout = 1.23 * (1 + (R2 / (R1 * R3))) where R1 = fixed resistor, R2 = potentiometer, R3 = added resistor

1.23 * (1 + (330 / (1800 * 1000))) = 

https://electronics.stackexchange.com/questions/276989/dac-controlled-buck-regulator

$I Rg = Vref / Rg$

$Vref = 1.23V$

$Rg = 330Ω$
https://electronics.stackexchange.com/questions/276989/dac-controlled-buck-regulator
$1.23 / 0.330 = Vref = 3.72mA$

$I Rdac = (Vref - Vdac) / Rdac$

$(1.23-0) / 1 = 1.23mA$

$I Rf = I Rg + I Rdac$

$3.72 + 1.23 = 4.93mA$

$Vout = Vref + I Rf * Rf$

$Rf = 1.374mA

1.23 + (4.93 * 1.374) = 8.00382V base line at 0 control volts

2.679 vattery pack voltage test

________________________

stuff laid out by Cluade - need to double check everything/continue researching

## Variable Bench Power Supply Schematic

```
INPUT POWER:
20V 3.25A Charger
    |
    ├─────────────────────────────────────┐
    |                                     |
    V                                     V
[Small Buck Module]              [LM2596 Buck Module]
Set to 5V fixed                  Adjustable output
    |                                     |
    |                                     |
    V                                     V
STM32F411CEU6                    [INA219 Module]
OLED Display (I2C)               Voltage/Current sensing
MCP4725 DAC (I2C)                    |
Rotary Encoder                       |
Power Switch Input                   V
    |                            [MOSFET Switch]
    |                            IRLZ44N
    |                            Controlled by STM32
    V                                |
GPIO Out ──10kΩ──> MOSFET Gate      |
Gate ──10kΩ──> GND                  V
                              OUTPUT TERMINALS
                              (Banana jacks/screw terminals)
```

## Detailed Connection Diagram

### POWER DISTRIBUTION:

```
20V Input
    ├─→ Small Buck → 5V → STM32 5V pin (or 3.3V to 3.3V pin)
    │                  └─→ OLED VCC
    │                  └─→ MCP4725 VCC
    │                  └─→ INA219 VCC
    │
    └─→ LM2596 Buck → Variable Output (3.3-12V)
```

### I2C BUS (all parallel):

```
STM32 I2C1:
    SDA (PB7 or PB9) ──┬─→ OLED SDA
                       ├─→ MCP4725 SDA
                       └─→ INA219 SDA
    
    SCL (PB6 or PB8) ──┬─→ OLED SCL
                       ├─→ MCP4725 SCL
                       └─→ INA219 SCL
```

All modules share:
    VCC → 5V or 3.3V (check module specs)
    GND → Common ground

### VOLTAGE CONTROL:

```
MCP4725 DAC Module:
    VCC → 5V
    GND → GND
    SDA → STM32 SDA
    SCL → STM32 SCL
    OUT → LM2596 FB pin (middle pot pad after removal)
```

### CURRENT/VOLTAGE MONITORING:

```
LM2596 Output (+) → INA219 VIN+
INA219 VIN- → MOSFET Drain
MOSFET Source → GND (common)

INA219 I2C:
    SDA/SCL → STM32 I2C bus
    VCC → 5V
    GND → GND
```

### POWER SWITCH:

```
STM32 GPIO (e.g., PA0):
    PA0 → 10kΩ resistor → MOSFET Gate (IRLZ44N)
    MOSFET Gate → 10kΩ pull-down → GND
    
MOSFET:
    Drain → INA219 output (VIN-)
    Source → GND
    Gate → from STM32 via 10kΩ
```

### USER INTERFACE (to be added)

```
Physical Power Switch:
    One terminal → 3.3V (from STM32)
    Other terminal → STM32 GPIO input (with internal pull-down)
    
Rotary Encoder (KY-040 or similar):
    CLK → STM32 GPIO (e.g., PA1)
    DT → STM32 GPIO (e.g., PA2)
    SW → STM32 GPIO (e.g., PA3)
    + → 3.3V
    GND → GND
    
Optional Buttons:
    Confirm Button → STM32 GPIO (pull-up or pull-down)
    Cancel Button → STM32 GPIO
```

## Wire Gauge Breakdown

Not quite - let me clarify the current path:

### Current Flow Path

```
Buck Converter OUT → INA219 IN → INA219 OUT → MOSFET → Output Terminals
     (3.25A)          (3.25A)       (3.25A)      (3.25A)     (3.25A)
```

All of these carry the same 3.25A load current.

Wire Gauge Recommendations
High-current path (all carry 3.25A):

- Buck OUT → INA219 IN: 18-20 AWG
- INA219 OUT → MOSFET Drain: 18-20 AWG
- MOSFET Source → GND: 18-20 AWG
- Output terminals connection: 16-18 AWG (slightly heavier for durability)

Low-current signal/control wires:

I- 2C (SDA, SCL): 22-26 AWG (milliamps)
- GPIO to MOSFET gate: 22-26 AWG (microamps)
- Encoder inputs: 22-26 AWG (milliamps)
- Power to MCU/modules (5V rail): 22-24 AWG (100-200mA total)

Summary
18-20 AWG for the entire main power path:

Buck output
Through INA219
Through MOSFET
To output terminals

22-26 AWG for all signal/control wires:

I2C communication
GPIO controls
Module power (low current)