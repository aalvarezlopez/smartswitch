# Flashing

monitor at91sam4 gpnvm set 1

# Hardware/Software interface

| Name                | Pin   | Direction  |
|---------------------|-------|------------|
| FlowMeter RAD1      | PB0   | INPUT (ISR)|
| FlowMeter RAD2      | PB1   | INPUT (ISR)|
| RAD1 valve          | PB2   | OUTPUT     |
| RAD2 valve          | PB3   | OUTPUT     |
| Shutter 1 (PWM )    | PA0   | OUTPUT     |
| Shutter 2 (PWM )    | PA1   | OUTPUT     |
| Shutter 3 (PWM )    | PA2   | OUTPUT     |
| Ethernet RESET      | PA6   | OUTPUT     |
| DIMMER (PWM 200 Hz) | PA7   | OTUPUT     |
| Touch sensor        | PA8   | INPUT (ISR)|
| PIR switch          | PA9   | INPUT (ISR)|
| Lights              | PA10  | OUTPUT     |
| AIR\_TEMP\_SENSOR   | PA16  | IN/OUTPUT  |
| LDR input           | PA19  | Anal. Input|
| RAD temperature     | PA20  | IN/OUTPUT  |


# Clock configuration

Input oscillator is 12 MHz as we have to generate 48 MHz for the USB.

OSC    -> Main Clock -> PLLA (x8) ->  PRES 1 -> MASTER & PER clock
12 Mhz -> 12 Mhz     -> 96 MHz    ->  96 MHz -> 96 MHz

Main clock input is driven by the PLLA at MHz

# Interrupts

List of interrupts which can be used by the system.

+ *SystemTick*: configured to be raised every 1ms. It's used to luanch the OS and
scheduler

# ADC conversion

There isjust one ADC channel which is channel 4. The adquisition shall be triggered
by software from the application and an interruption will raise once the result
is available.

Channel 4 is enabled at start-up and then we only can trigger a new acquisition. If we
would like to start an acquisiton in other channel we would need to create a new
function which allows us to reconfigure the ADC.

ADCCLK is configure to 2 MHz, which is MAINCLOCK (96 MHz) divided by 48 (PRESCAL).

+ Start-up time is 96 * ADCCLCK = 48 us
+ TRACKING time is 0 as we don't have more than one channel
+ Settling time is 5 * ADCCLK = 2.5 us

# Human interface

**To be completed**

# ToDo


# Flash and debug

'''
openocd -f viquina_jtag.cfg
gdb-multiarch ./smartSwitch.elf
gdb-multiarch -x ./gdbinit.gdb ./smartSwitch.elf
'''

'''
target remote localhost:3333
load
'''
