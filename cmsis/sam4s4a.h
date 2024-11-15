/**
 * \file
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _SAM4S4A_
#define _SAM4S4A_

/** \addtogroup SAM4S4A_definitions SAM4S4A definitions
  This file defines all structures and symbols for SAM4S4A:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
*/
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include <stdint.h>
#endif
#include <stdbool.h>

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */
/** \addtogroup SAM4S4A_cmsis CMSIS Definitions */
/*@{*/

/**< Interrupt Number Definition */
typedef enum IRQn {
    /******  Cortex-M4 Processor Exceptions Numbers ******************************/
    NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
    MemoryManagement_IRQn = -12, /**<  4 Cortex-M4 Memory Management Interrupt */
    BusFault_IRQn         = -11, /**<  5 Cortex-M4 Bus Fault Interrupt         */
    UsageFault_IRQn       = -10, /**<  6 Cortex-M4 Usage Fault Interrupt       */
    SVCall_IRQn           = -5,  /**< 11 Cortex-M4 SV Call Interrupt           */
    DebugMonitor_IRQn     = -4,  /**< 12 Cortex-M4 Debug Monitor Interrupt     */
    PendSV_IRQn           = -2,  /**< 14 Cortex-M4 Pend SV Interrupt           */
    SysTick_IRQn          = -1,  /**< 15 Cortex-M4 System Tick Interrupt       */
    /******  SAM4S4A specific Interrupt Numbers *********************************/

    SUPC_IRQn            =  0, /**<  0 SAM4S4A Supply Controller (SUPC) */
    RSTC_IRQn            =  1, /**<  1 SAM4S4A Reset Controller (RSTC) */
    RTC_IRQn             =  2, /**<  2 SAM4S4A Real Time Clock (RTC) */
    RTT_IRQn             =  3, /**<  3 SAM4S4A Real Time Timer (RTT) */
    WDT_IRQn             =  4, /**<  4 SAM4S4A Watchdog Timer (WDT) */
    PMC_IRQn             =  5, /**<  5 SAM4S4A Power Management Controller (PMC) */
    EFC0_IRQn            =  6, /**<  6 SAM4S4A Enhanced Embedded Flash Controller 0 (EFC0) */
    UART0_IRQn           =  8, /**<  8 SAM4S4A UART 0 (UART0) */
    UART1_IRQn           =  9, /**<  9 SAM4S4A UART 1 (UART1) */
    PIOA_IRQn            = 11, /**< 11 SAM4S4A Parallel I/O Controller A (PIOA) */
    PIOB_IRQn            = 12, /**< 12 SAM4S4A Parallel I/O Controller B (PIOB) */
    USART0_IRQn          = 14, /**< 14 SAM4S4A USART 0 (USART0) */
    TWI0_IRQn            = 19, /**< 19 SAM4S4A Two Wire Interface 0 (TWI0) */
    TWI1_IRQn            = 20, /**< 20 SAM4S4A Two Wire Interface 1 (TWI1) */
    SPI_IRQn             = 21, /**< 21 SAM4S4A Serial Peripheral Interface (SPI) */
    SSC_IRQn             = 22, /**< 22 SAM4S4A Synchronous Serial Controller (SSC) */
    TC0_IRQn             = 23, /**< 23 SAM4S4A Timer/Counter 0 (TC0) */
    TC1_IRQn             = 24, /**< 24 SAM4S4A Timer/Counter 1 (TC1) */
    TC2_IRQn             = 25, /**< 25 SAM4S4A Timer/Counter 2 (TC2) */
    ADC_IRQn             = 29, /**< 29 SAM4S4A Analog To Digital Converter (ADC) */
    PWM_IRQn             = 31, /**< 31 SAM4S4A Pulse Width Modulation (PWM) */
    CRCCU_IRQn           = 32, /**< 32 SAM4S4A CRC Calculation Unit (CRCCU) */
    ACC_IRQn             = 33, /**< 33 SAM4S4A Analog Comparator (ACC) */
    UDP_IRQn             = 34, /**< 34 SAM4S4A USB Device Port (UDP) */

    PERIPH_COUNT_IRQn    = 35  /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors {
    /* Stack pointer */
    void* pvStack;

    /* Cortex-M handlers */
    void* pfnReset_Handler;
    void* pfnNMI_Handler;
    void* pfnHardFault_Handler;
    void* pfnMemManage_Handler;
    void* pfnBusFault_Handler;
    void* pfnUsageFault_Handler;
    void* pfnReserved1_Handler;
    void* pfnReserved2_Handler;
    void* pfnReserved3_Handler;
    void* pfnReserved4_Handler;
    void* pfnSVC_Handler;
    void* pfnDebugMon_Handler;
    void* pfnReserved5_Handler;
    void* pfnPendSV_Handler;
    void* pfnSysTick_Handler;

    /* Peripheral handlers */
    void* pfnSUPC_Handler;   /*  0 Supply Controller */
    void* pfnRSTC_Handler;   /*  1 Reset Controller */
    void* pfnRTC_Handler;    /*  2 Real Time Clock */
    void* pfnRTT_Handler;    /*  3 Real Time Timer */
    void* pfnWDT_Handler;    /*  4 Watchdog Timer */
    void* pfnPMC_Handler;    /*  5 Power Management Controller */
    void* pfnEFC0_Handler;   /*  6 Enhanced Embedded Flash Controller 0 */
    void* pvReserved7;
    void* pfnUART0_Handler;  /*  8 UART 0 */
    void* pfnUART1_Handler;  /*  9 UART 1 */
    void* pvReserved10;
    void* pfnPIOA_Handler;   /* 11 Parallel I/O Controller A */
    void* pfnPIOB_Handler;   /* 12 Parallel I/O Controller B */
    void* pvReserved13;
    void* pfnUSART0_Handler; /* 14 USART 0 */
    void* pvReserved15;
    void* pvReserved16;
    void* pvReserved17;
    void* pvReserved18;
    void* pfnTWI0_Handler;   /* 19 Two Wire Interface 0 */
    void* pfnTWI1_Handler;   /* 20 Two Wire Interface 1 */
    void* pfnSPI_Handler;    /* 21 Serial Peripheral Interface */
    void* pfnSSC_Handler;    /* 22 Synchronous Serial Controller */
    void* pfnTC0_Handler;    /* 23 Timer/Counter 0 */
    void* pfnTC1_Handler;    /* 24 Timer/Counter 1 */
    void* pfnTC2_Handler;    /* 25 Timer/Counter 2 */
    void* pvReserved26;
    void* pvReserved27;
    void* pvReserved28;
    void* pfnADC_Handler;    /* 29 Analog To Digital Converter */
    void* pvReserved30;
    void* pfnPWM_Handler;    /* 31 Pulse Width Modulation */
    void* pfnCRCCU_Handler;  /* 32 CRC Calculation Unit */
    void* pfnACC_Handler;    /* 33 Analog Comparator */
    void* pfnUDP_Handler;    /* 34 USB Device Port */
} DeviceVectors;

/* Cortex-M4 core handlers */
void Reset_Handler      ( void );
void NMI_Handler        ( void );
void HardFault_Handler  ( void );
void MemManage_Handler  ( void );
void BusFault_Handler   ( void );
void UsageFault_Handler ( void );
void SVC_Handler        ( void );
void DebugMon_Handler   ( void );
void PendSV_Handler     ( void );
void SysTick_Handler    ( void );

/* Peripherals handlers */
void ACC_Handler        ( void );
void ADC_Handler        ( void );
void CRCCU_Handler      ( void );
void EFC0_Handler       ( void );
void PIOA_Handler       ( void );
void PIOB_Handler       ( void );
void PMC_Handler        ( void );
void PWM_Handler        ( void );
void RSTC_Handler       ( void );
void RTC_Handler        ( void );
void RTT_Handler        ( void );
void SPI_Handler        ( void );
void SSC_Handler        ( void );
void SUPC_Handler       ( void );
void TC0_Handler        ( void );
void TC1_Handler        ( void );
void TC2_Handler        ( void );
void TWI0_Handler       ( void );
void TWI1_Handler       ( void );
void UART0_Handler      ( void );
void UART1_Handler      ( void );
void UDP_Handler        ( void );
void USART0_Handler     ( void );
void WDT_Handler        ( void );

/**
 * \brief Configuration of the Cortex-M4 Processor and Core Peripherals
 */

#define __CM4_REV              0x0001 /**< SAM4S4A core revision number ([15:8] revision number, [7:0] patch number) */
#define __MPU_PRESENT          1      /**< SAM4S4A does provide a MPU */
#define __FPU_PRESENT          0      /**< SAM4S4A does not provide a FPU */
#define __NVIC_PRIO_BITS       4      /**< SAM4S4A uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick Config is used */

/*
 * \brief CMSIS includes
 */

#include <core_cm4.h>
#if !defined DONT_USE_CMSIS_INIT
#include "system_sam4s.h"
#endif /* DONT_USE_CMSIS_INIT */

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAM4S4A */
/* ************************************************************************** */
/** \addtogroup SAM4S4A_api Peripheral Software API */
/*@{*/

#include "component/component_acc.h"
#include "component/component_adc.h"
#include "component/component_chipid.h"
#include "component/component_crccu.h"
#include "component/component_efc.h"
#include "component/component_gpbr.h"
#include "component/component_matrix.h"
#include "component/component_pdc.h"
#include "component/component_pio.h"
#include "component/component_pmc.h"
#include "component/component_pwm.h"
#include "component/component_rstc.h"
#include "component/component_rtc.h"
#include "component/component_rtt.h"
#include "component/component_spi.h"
#include "component/component_ssc.h"
#include "component/component_supc.h"
#include "component/component_tc.h"
#include "component/component_twi.h"
#include "component/component_uart.h"
#include "component/component_udp.h"
#include "component/component_usart.h"
#include "component/component_wdt.h"
/*@}*/

/* ************************************************************************** */
/*   REGISTER ACCESS DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */
/** \addtogroup SAM4S4A_reg Registers Access Definitions */
/*@{*/

#include "instance/instance_ssc.h"
#include "instance/instance_spi.h"
#include "instance/instance_tc0.h"
#include "instance/instance_twi0.h"
#include "instance/instance_twi1.h"
#include "instance/instance_pwm.h"
#include "instance/instance_usart0.h"
#include "instance/instance_udp.h"
#include "instance/instance_adc.h"
#include "instance/instance_acc.h"
#include "instance/instance_crccu.h"
#include "instance/instance_matrix.h"
#include "instance/instance_pmc.h"
#include "instance/instance_uart0.h"
#include "instance/instance_chipid.h"
#include "instance/instance_uart1.h"
#include "instance/instance_efc0.h"
#include "instance/instance_pioa.h"
#include "instance/instance_piob.h"
#include "instance/instance_rstc.h"
#include "instance/instance_supc.h"
#include "instance/instance_rtt.h"
#include "instance/instance_wdt.h"
#include "instance/instance_rtc.h"
#include "instance/instance_gpbr.h"
/*@}*/

/* ************************************************************************** */
/*   PERIPHERAL ID DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */
/** \addtogroup SAM4S4A_id Peripheral Ids Definitions */
/*@{*/

#define ID_SUPC   ( 0) /**< \brief Supply Controller (SUPC) */
#define ID_RSTC   ( 1) /**< \brief Reset Controller (RSTC) */
#define ID_RTC    ( 2) /**< \brief Real Time Clock (RTC) */
#define ID_RTT    ( 3) /**< \brief Real Time Timer (RTT) */
#define ID_WDT    ( 4) /**< \brief Watchdog Timer (WDT) */
#define ID_PMC    ( 5) /**< \brief Power Management Controller (PMC) */
#define ID_EFC0   ( 6) /**< \brief Enhanced Embedded Flash Controller 0 (EFC0) */
#define ID_UART0  ( 8) /**< \brief UART 0 (UART0) */
#define ID_UART1  ( 9) /**< \brief UART 1 (UART1) */
#define ID_PIOA   (11) /**< \brief Parallel I/O Controller A (PIOA) */
#define ID_PIOB   (12) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_USART0 (14) /**< \brief USART 0 (USART0) */
#define ID_TWI0   (19) /**< \brief Two Wire Interface 0 (TWI0) */
#define ID_TWI1   (20) /**< \brief Two Wire Interface 1 (TWI1) */
#define ID_SPI    (21) /**< \brief Serial Peripheral Interface (SPI) */
#define ID_SSC    (22) /**< \brief Synchronous Serial Controller (SSC) */
#define ID_TC0    (23) /**< \brief Timer/Counter 0 (TC0) */
#define ID_TC1    (24) /**< \brief Timer/Counter 1 (TC1) */
#define ID_TC2    (25) /**< \brief Timer/Counter 2 (TC2) */
#define ID_ADC    (29) /**< \brief Analog To Digital Converter (ADC) */
#define ID_PWM    (31) /**< \brief Pulse Width Modulation (PWM) */
#define ID_CRCCU  (32) /**< \brief CRC Calculation Unit (CRCCU) */
#define ID_ACC    (33) /**< \brief Analog Comparator (ACC) */
#define ID_UDP    (34) /**< \brief USB Device Port (UDP) */

#define ID_PERIPH_COUNT (35) /**< \brief Number of peripheral IDs */
/*@}*/

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */
/** \addtogroup SAM4S4A_base Peripheral Base Address Definitions */
/*@{*/

#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#else
#define SSC        ((Ssc    *)0x40004000U) /**< \brief (SSC       ) Base Address */
#define PDC_SSC    ((Pdc    *)0x40004100U) /**< \brief (PDC_SSC   ) Base Address */
#define SPI        ((Spi    *)0x40008000U) /**< \brief (SPI       ) Base Address */
#define PDC_SPI    ((Pdc    *)0x40008100U) /**< \brief (PDC_SPI   ) Base Address */
#define TC0        ((Tc     *)0x40010000U) /**< \brief (TC0       ) Base Address */
#define TWI0       ((Twi    *)0x40018000U) /**< \brief (TWI0      ) Base Address */
#define PDC_TWI0   ((Pdc    *)0x40018100U) /**< \brief (PDC_TWI0  ) Base Address */
#define TWI1       ((Twi    *)0x4001C000U) /**< \brief (TWI1      ) Base Address */
#define PDC_TWI1   ((Pdc    *)0x4001C100U) /**< \brief (PDC_TWI1  ) Base Address */
#define PWM        ((Pwm    *)0x40020000U) /**< \brief (PWM       ) Base Address */
#define PDC_PWM    ((Pdc    *)0x40020100U) /**< \brief (PDC_PWM   ) Base Address */
#define USART0     ((Usart  *)0x40024000U) /**< \brief (USART0    ) Base Address */
#define PDC_USART0 ((Pdc    *)0x40024100U) /**< \brief (PDC_USART0) Base Address */
#define UDP        ((Udp    *)0x40034000U) /**< \brief (UDP       ) Base Address */
#define ADC        ((Adc    *)0x40038000U) /**< \brief (ADC       ) Base Address */
#define PDC_ADC    ((Pdc    *)0x40038100U) /**< \brief (PDC_ADC   ) Base Address */
#define ACC        ((Acc    *)0x40040000U) /**< \brief (ACC       ) Base Address */
#define CRCCU      ((Crccu  *)0x40044000U) /**< \brief (CRCCU     ) Base Address */
#define MATRIX     ((Matrix *)0x400E0200U) /**< \brief (MATRIX    ) Base Address */
#define PMC        ((Pmc    *)0x400E0400U) /**< \brief (PMC       ) Base Address */
#define UART0      ((Uart   *)0x400E0600U) /**< \brief (UART0     ) Base Address */
#define PDC_UART0  ((Pdc    *)0x400E0700U) /**< \brief (PDC_UART0 ) Base Address */
#define CHIPID     ((Chipid *)0x400E0740U) /**< \brief (CHIPID    ) Base Address */
#define UART1      ((Uart   *)0x400E0800U) /**< \brief (UART1     ) Base Address */
#define PDC_UART1  ((Pdc    *)0x400E0900U) /**< \brief (PDC_UART1 ) Base Address */
#define EFC0       ((Efc    *)0x400E0A00U) /**< \brief (EFC0      ) Base Address */
#define PIOA       ((Pio    *)0x400E0E00U) /**< \brief (PIOA      ) Base Address */
#define PDC_PIOA   ((Pdc    *)0x400E0F68U) /**< \brief (PDC_PIOA  ) Base Address */
#define PIOB       ((Pio    *)0x400E1000U) /**< \brief (PIOB      ) Base Address */
#define RSTC       ((Rstc   *)0x400E1400U) /**< \brief (RSTC      ) Base Address */
#define SUPC       ((Supc   *)0x400E1410U) /**< \brief (SUPC      ) Base Address */
#define RTT        ((Rtt    *)0x400E1430U) /**< \brief (RTT       ) Base Address */
#define WDT        ((Wdt    *)0x400E1450U) /**< \brief (WDT       ) Base Address */
#define RTC        ((Rtc    *)0x400E1460U) /**< \brief (RTC       ) Base Address */
#define GPBR       ((Gpbr   *)0x400E1490U) /**< \brief (GPBR      ) Base Address */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/*@}*/

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */
/** \addtogroup SAM4S4A_pio Peripheral Pio Definitions */
/*@{*/

#include "pio/pio_sam4s4a.h"
/*@}*/

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */

#define IFLASH0_SIZE             (0x40000u)
#define IFLASH0_PAGE_SIZE        (512u)
#define IFLASH0_LOCK_REGION_SIZE (8192u)
#define IFLASH0_NB_OF_PAGES      (512u)
#define IFLASH0_NB_OF_LOCK_BITS  (32u)
#define IRAM_SIZE                (0x10000u)
#define IFLASH_SIZE              (IFLASH0_SIZE)

#define IFLASH0_ADDR (0x00400000u) /**< Internal Flash 0 base address */
#define IROM_ADDR    (0x00800000u) /**< Internal ROM base address */
#define IRAM_ADDR    (0x20000000u) /**< Internal RAM base address */
#define EBI_CS0_ADDR (0x60000000u) /**< EBI Chip Select 0 base address */
#define EBI_CS1_ADDR (0x61000000u) /**< EBI Chip Select 1 base address */
#define EBI_CS2_ADDR (0x62000000u) /**< EBI Chip Select 2 base address */
#define EBI_CS3_ADDR (0x63000000u) /**< EBI Chip Select 3 base address */

/* ************************************************************************** */
/*   MISCELLANEOUS DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */

#define CHIP_JTAGID       (0x05B3203FUL)
#define CHIP_CIDR         (0x288B09E0UL)
#define CHIP_EXID         (0x0UL)
#define NB_CH_ADC         (7UL)
#define NB_CH_DAC         (-UL)
#define USB_DEVICE_MAX_EP (8UL)

/* ************************************************************************** */
/*   ELECTRICAL DEFINITIONS FOR SAM4S4A */
/* ************************************************************************** */

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN           (20000UL)
#define CHIP_FREQ_SLCK_RC               (32000UL)
#define CHIP_FREQ_SLCK_RC_MAX           (44000UL)
#define CHIP_FREQ_MAINCK_RC_4MHZ        (4000000UL)
#define CHIP_FREQ_MAINCK_RC_8MHZ        (8000000UL)
#define CHIP_FREQ_MAINCK_RC_12MHZ       (12000000UL)
#define CHIP_FREQ_CPU_MAX               (120000000UL)
#define CHIP_FREQ_XTAL_32K              (32768UL)

/* Embedded Flash Write Wait State */
#define CHIP_FLASH_WRITE_WAIT_STATE     (6U)

/* Embedded Flash Read Wait State (VDDCORE set at 1.08V and VDDIO 3.3V) */
#define CHIP_FREQ_FWS_0                 (20000000UL)  /**< \brief Maximum operating frequency when FWS is 0 */
#define CHIP_FREQ_FWS_1                 (40000000UL)  /**< \brief Maximum operating frequency when FWS is 1 */
#define CHIP_FREQ_FWS_2                 (60000000UL)  /**< \brief Maximum operating frequency when FWS is 2 */
#define CHIP_FREQ_FWS_3                 (80000000UL)  /**< \brief Maximum operating frequency when FWS is 3 */
#define CHIP_FREQ_FWS_4                 (100000000UL) /**< \brief Maximum operating frequency when FWS is 4 */
#define CHIP_FREQ_FWS_5                 (123000000UL) /**< \brief Maximum operating frequency when FWS is 5 */

/* HYSTeresis levels: please refer to Electrical Characteristics */
#define ACC_ACR_HYST_50MV_MAX           (0x01UL)
#define ACC_ACR_HYST_90MV_MAX           (0x11UL)

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* _SAM4S4A_ */
