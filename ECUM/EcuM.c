#include "stdint.h"
#include "EcuM.h"
#include "EcuM_pri.h"
#include "sam4s4a.h"
#include "component_pio.h"
#include "spi.h"
#include "smartswitch.h"
#include "isr.h"
#include "display.h"
#include "enc28j60.h"
#include "delays.h"
#include "uart.h"

#define ADC_PRESCAL_VALUE_2_MHz 96u

extern uint32_t system_tick_counter;
uint32_t system_tick_counter_ref = 0;

/**
 * @brief Start drivers and other modules
 */
void EcuM_Startup_one(void)
{
    ecum_configure_peripheral_clocks();
    ecum_configure_io_interfaces();
    ecum_configure_uart_interface();
    ecum_configure_spi_interface();
    ecum_configure_pwm_interface();
    ecum_configure_analog_input_interface();

    IO_PWM_Init();

    DS18B20_Init();

    /* ToDo: Implement the WDG module, we are disabling the
     * WDG peripheral for now
     */
    WDT->WDT_MR &= ~(WDT_MR_WDRSTEN);

    SPI_Init();
    I2C_Init();
}

void EcuM_Startup_two(void)
{
    uint8_t lastSequence = 0;
    uint8_t writeBuffer[16];

    ENC_Init();
    Display_Init();
    SmartSwitch_Init();
    TCPIP_Init();
    Rtc_setTimeDate(22, 2, 2, 8, 15, 20);

    UART_Init();

    ISR_setInterruptEnable( UART0_IRQn, true);
    ISR_setInterruptEnable( ADC_IRQn, true);
    ISR_setInterruptEnable( PIOA_IRQn, true);
    ISR_setInterruptEnable( PIOB_IRQn, true);
}

uint32_t EcuM_GetCurrentCounter(void)
{
    system_tick_counter_ref = system_tick_counter;
    return system_tick_counter;
}

uint32_t EcuM_GetElapsed(void)
{
    return (system_tick_counter - system_tick_counter_ref);
}


/**
 * @brief Get CPU main clock frequency in MHz
 *
 * @return Main clock frequency
 */
uint8_t EcuM_getMainClockSpeed(void)
{
    return ECUM_MAIN_CLOCK_FREQ;
}


/**
 * @brief Configure inputs, outputs and other
 * pins
 *
 * Enable protected register written before
 * writing to the registers and then protect
 * again the registers
 *
 * See Readme.md for more information about the
 * pin configuration
 *
 */
void ecum_configure_io_interfaces(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;

    PIOA->PIO_PER = PIO_PER_P0  | PIO_PER_P1  | PIO_PER_P2  | PIO_PER_P6  |
                    PIO_PER_P7  | PIO_PER_P8  | PIO_PER_P17 | PIO_PER_P18 |
                    PIO_PER_P19 | PIO_PER_P20;

    PIOA->PIO_OER = PIO_PER_P0  | PIO_PER_P1  | PIO_PER_P2  | PIO_PER_P6 |
                    PIO_PER_P7  | PIO_PER_P17;
    PIOA->PIO_ODR = PIO_PER_P19;

    PIOA->PIO_PDR = PIO_PER_P4 | PIO_PER_P3;
    PIOA->PIO_PDR = PIO_PER_P9 | PIO_PER_P10;
    PIOA->PIO_PDR = PIO_PER_P11 | PIO_PER_P12;
    PIOA->PIO_PDR = PIO_PER_P13 | PIO_PER_P14;
    PIOA->PIO_PDR = PIO_PER_P0 | PIO_PER_P1 | PIO_PER_P2 | PIO_PER_P7;
    PIOA->PIO_MDER = PIO_PER_P0 | PIO_PER_P1 | PIO_PER_P2 | PIO_PER_P7;

    PIOA->PIO_ABCDSR[0] &= ~PIO_ABCDSR_P3;
    PIOA->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P3;

    PIOA->PIO_ABCDSR[0] &= ~PIO_ABCDSR_P4;
    PIOA->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P4;

    PIOA->PIO_CODR = PIO_CODR_P0  | PIO_CODR_P1  | PIO_CODR_P2  | PIO_CODR_P6 |
                     PIO_CODR_P7  | PIO_CODR_P10 | PIO_CODR_P17;
    PIOA->PIO_SODR = PIO_SODR_P6;

    PIOA->PIO_IER    = PIO_IER_P18 | PIO_IER_P8;
    PIOA->PIO_PUER   = PIO_PUER_P18 | PIO_PUER_P8;
    PIOA->PIO_IFER   = PIO_IFSR_P18 | PIO_IFSR_P8;
    PIOA->PIO_IFSCER = PIO_IFSCDR_P18 | PIO_IFSCDR_P8;

    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;

    PIOB->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;

    PIOB->PIO_PER = PIO_PER_P0 | PIO_PER_P1 |
                    PIO_PER_P2 | PIO_PER_P3;
    PIOB->PIO_OER = PIO_OER_P2 | PIO_OER_P3;
    PIOB->PIO_ODR = PIO_ODR_P0 | PIO_ODR_P1;

    PIOB->PIO_IER    = PIO_IER_P0    | PIO_IER_P1;
    PIOB->PIO_PUER   = PIO_PUER_P0   | PIO_PUER_P1;
    PIOB->PIO_IFER   = PIO_IFSR_P0   | PIO_IFSR_P1;
    PIOB->PIO_IFSCER = PIO_IFSCDR_P0 | PIO_IFSCDR_P1;

    PIOB->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}


void ecum_configure_pwm_interface(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;

    /* PA0 -> PWM0 PER A */
    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P0);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P0);
    /* PA1 -> PWM1  PER A*/
    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P0);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P0);
    /* PA2 -> PWM2  PER A*/
    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P0);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P0);
    /* PA7 -> PWM3  PER B*/
    PIOA->PIO_ABCDSR[0] |= (PIO_ABCDSR_P7);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P7);

    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}
/**
 * @brief configure SPI interface to communicate with the LCD
 *
 * CS -> PA11
 * SDO -> PA12
 * SDI -> PA13
 * SCL -> PA14
 */
void ecum_configure_spi_interface(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
    /* Assure peripheral A is selected for SPI pins */
    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P11);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P11);

    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P12);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P12);

    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P13);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P13);

    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P14);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P14);

    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}


/**
 * @brief Configure uart interface to communicate with extensions
 * RX -> PB2
 * TX -> PB3
 */
void ecum_configure_uart_interface(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
    /*Assure Tx/Rx pins are selected for UART peripheral*/
    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P9);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P10);

    PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P9);
    PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P10);

    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;

}


/**
 * @brief Configure analog capture interface and peripheral
 * BATTERY_LEVEL -> PB0/AD4
 */
void ecum_configure_analog_input_interface(void)
{
    ADC->ADC_WPMR = ADC_WPMR_WPKEY_PASSWD;
    ADC->ADC_CHER = ADC_CHER_CH2;
    /* configure acquisition mode
     * ADCLCK = MAIN_CLOCK / PRESCAL
     */
    ADC->ADC_MR =  ADC_MR_PRESCAL( ADC_PRESCAL_VALUE_2_MHz ) |
                   ADC_MR_TRGEN_DIS |
                   ADC_MR_SLEEP_NORMAL |
                   ADC_MR_FREERUN_OFF |
                   ADC_MR_STARTUP_SUT96 |
                   ADC_MR_SETTLING_AST5 |
                   ADC_MR_ANACH_NONE |
                   ADC_MR_TRACKTIM(0) |
                   ADC_MR_TRANSFER(2) |    /* mandatory to set it to 2*/
                   ADC_MR_USEQ_NUM_ORDER;

    ADC->ADC_COR = 0;  /* No offset and single mode*/
    ADC->ADC_CGR = 0; /* All gains set to 1 */

    ADC->ADC_EMR |= ADC_EMR_TAG;

    ADC->ADC_IER = ADC_IER_DRDY;

    ADC->ADC_WPMR = ADC_WPMR_WPEN |
                    ADC_WPMR_WPKEY_PASSWD;
}


/**
 * @brief Enable peripheral clocks
 *
 * + ID8 for UART0
 * + ID11 for PIOA
 * + ID12 for PIOB
 * + ID21 for SPI
 * + ID24 for ADC
 * + ID34 for UDP (USB)
 */
void ecum_configure_peripheral_clocks(void)
{
    /* Unlock PMC registers*/
    PMC->PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;
    PMC->PMC_PCER0 = PMC_PCER0_PID8  | PMC_PCER0_PID11 | PMC_PCER0_PID12 |
                     PMC_PCER0_PID19 | PMC_PCER0_PID21 | PMC_PCER0_PID29 |
                     PMC_PCER0_PID31;
    /* Lock PMC registers*/
    PMC->PMC_WPMR =   PMC_WPMR_WPEN |
                      PMC_WPMR_WPKEY_PASSWD;
}
