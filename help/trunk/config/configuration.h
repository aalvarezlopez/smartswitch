/*
 * configuration.h
 *
 *  Created on: Nov 20, 2013
 *      Author: Adrian
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#define BOARD_VERSION   1

//ToDo change to SPI header file
//#include "component/component_spi.h"
#define CS0     0b0 << SPI_TDR_PCS_Pos
#define CS1     0b1 << SPI_TDR_PCS_Pos
#define CS2     0b11 << SPI_TDR_PCS_Pos
#define CS3     0b111 << SPI_TDR_PCS_Pos

#define CS_SD   CS1

#define BAUDRATE 115200
#define CPUCLOCK  48000000
#define CD_BAUDRATE (CPUCLOCK)/(BAUDRATE * 16)
#define USART_PORT 1

//PIN DEFINES
#define SAMPLINGPIN         2
#define WAKEUPPIN           3
#define BAT_MEAS            4
#define CLK_ALARM           5
#define CLK_EIC             3
#define VIB_ALARM1          7
#define VIB1_EIC            4
#define VIB_ALARM2          6
#define VIB2_EIC            1
#define CRCT_ON_PIN         8
#define AVDD_ON_PIN         10
#define CHRG_DET            11
#define VBUSDET             12
#if BOARD_VERSION == 1
    #define LEDGREEN        20
    #define LEDRED          19
#else
    #define LEDGREEN        13
    #define LEDRED          14
#endif
#define SOFTCS3             15  //PGA113
#define SOFTCS2             16  //LIS3DH
#define SDAPIN              21
#define SCLPIN              22
#define USBDMPIN            25
#define USBDPPIN            26
#define MISOPIN             27
#define MOSIPIN             28
#define MSCKPIN             29
#if BOARD_VERSION == 1
    #define SOFTCS0         17  //ADC
#else
    #define SOFTCS0         30
#endif
#define SOFTCS1             31

#define PORTSIZE            32
#define SDDETECTIONPIN      45
#define PULSEPIN            88

#if BOARD_VERSION == 1
    #define BBMISO          13
    #define BBMOSI          14
    #define BBCLK           18
#else
    #define BBMISO          MISOPIN
    #define BBMOSI          MOSIPIN
    #define BBCLK           MSCKPIN
#endif
#if BOARD_VERSION == 1
    #define SDPWR           30
#endif


#define ADC_CLOCK_DIVISOR 100
#define COUNTER4TMRCLK2   1899

#define SAMPLEPRIORITY  1
#define STRING_HEADER "-- VibLog App --\r\n" \
    "-- Compiled: "__DATE__" "__TIME__" --\r\n"

#define BASE_SAMPLE_RATE 12800

#endif /* CONFIGURATION_H_ */
