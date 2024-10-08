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
| PIR switch          | PA18  | INPUT (ISR)|
| Lights              | PA17  | OUTPUT     |
| AIR\_TEMP\_SENSOR   | PA16  | IN/OUTPUT  |
| LDR input           | PA19  | Anal. Input|
| RAD temperature     | PA20  | IN/OUTPUT  |

### RAD1 valve

Normally open contactor. The output is drived by a relay.

### RAD2 valve

Open collector output. Output is tied to GROUND when active.

### Lights

Normally closed contactor. The output is drived by a realy.


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

## Display

Display driver is SH1106. Size is 128x64 pixeles.

8 pages, page width is 128 bytes (*128x8x8=8192, which is equal to 128x64*).


## Coms

Udp message. Periodically the device broadcast status message: 

[HH:MM:SS]{TEMP: x,
RAD=ON/OFF,
LIGHTS=ON/OFF,
SHUTTER:x;y;z}

Device will accept these commands:

LIGHT:ON
LIGHT:OFF
TTARGET:27
SHUTTER:76
STATUS
DATE:YY/MM/DD/HH/MM/SS


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

# Satellite

Satellaties allow to have more than one switch handling the same light. They communicate with
this devices using UART. Each one have an Id from 1 to 5.

The master (this device) will transmit periodically one message to each id with the
format "SHS090X{dimmer:Y}", where X is the Id and Y the dimmer (from 0 to 100).

Each device will reply only if the Id match its own id. The reply format is "SHS09X{Y}"
where X is the Id and Y is "True" or "False". True means that the button in the satellite was
pressend since the last query.

