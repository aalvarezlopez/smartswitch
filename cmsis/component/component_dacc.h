/**
 * \file
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _SAM4S_DACC_COMPONENT_
#define _SAM4S_DACC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Digital-to-Analog Converter Controller */
/* ============================================================================= */
/** \addtogroup SAM4S_DACC Digital-to-Analog Converter Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Dacc hardware registers */
typedef struct {
    __O  uint32_t DACC_CR;       /**< \brief (Dacc Offset: 0x00) Control Register */
    __IO uint32_t DACC_MR;       /**< \brief (Dacc Offset: 0x04) Mode Register */
    __I  uint32_t Reserved1[2];
    __O  uint32_t
    DACC_CHER;     /**< \brief (Dacc Offset: 0x10) Channel Enable Register */
    __O  uint32_t
    DACC_CHDR;     /**< \brief (Dacc Offset: 0x14) Channel Disable Register */
    __I  uint32_t
    DACC_CHSR;     /**< \brief (Dacc Offset: 0x18) Channel Status Register */
    __I  uint32_t Reserved2[1];
    __O  uint32_t
    DACC_CDR;      /**< \brief (Dacc Offset: 0x20) Conversion Data Register */
    __O  uint32_t
    DACC_IER;      /**< \brief (Dacc Offset: 0x24) Interrupt Enable Register */
    __O  uint32_t
    DACC_IDR;      /**< \brief (Dacc Offset: 0x28) Interrupt Disable Register */
    __I  uint32_t
    DACC_IMR;      /**< \brief (Dacc Offset: 0x2C) Interrupt Mask Register */
    __I  uint32_t
    DACC_ISR;      /**< \brief (Dacc Offset: 0x30) Interrupt Status Register */
    __I  uint32_t Reserved3[24];
    __IO uint32_t
    DACC_ACR;      /**< \brief (Dacc Offset: 0x94) Analog Current Register */
    __I  uint32_t Reserved4[19];
    __IO uint32_t
    DACC_WPMR;     /**< \brief (Dacc Offset: 0xE4) Write Protect Mode register */
    __I  uint32_t
    DACC_WPSR;     /**< \brief (Dacc Offset: 0xE8) Write Protect Status register */
    __I  uint32_t Reserved5[7];
    __IO uint32_t
    DACC_TPR;      /**< \brief (Dacc Offset: 0x108) Transmit Pointer Register */
    __IO uint32_t
    DACC_TCR;      /**< \brief (Dacc Offset: 0x10C) Transmit Counter Register */
    __I  uint32_t Reserved6[2];
    __IO uint32_t
    DACC_TNPR;     /**< \brief (Dacc Offset: 0x118) Transmit Next Pointer Register */
    __IO uint32_t
    DACC_TNCR;     /**< \brief (Dacc Offset: 0x11C) Transmit Next Counter Register */
    __O  uint32_t
    DACC_PTCR;     /**< \brief (Dacc Offset: 0x120) Transfer Control Register */
    __I  uint32_t
    DACC_PTSR;     /**< \brief (Dacc Offset: 0x124) Transfer Status Register */
} Dacc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- DACC_CR : (DACC Offset: 0x00) Control Register -------- */
#define DACC_CR_SWRST (0x1u << 0) /**< \brief (DACC_CR) Software Reset */
/* -------- DACC_MR : (DACC Offset: 0x04) Mode Register -------- */
#define DACC_MR_TRGEN (0x1u << 0) /**< \brief (DACC_MR) Trigger Enable */
#define   DACC_MR_TRGEN_DIS (0x0u << 0) /**< \brief (DACC_MR) External trigger mode disabled. DACC in free running mode. */
#define   DACC_MR_TRGEN_EN (0x1u << 0) /**< \brief (DACC_MR) External trigger mode enabled. */
#define DACC_MR_TRGSEL_Pos 1
#define DACC_MR_TRGSEL_Msk (0x7u << DACC_MR_TRGSEL_Pos) /**< \brief (DACC_MR) Trigger Selection */
#define DACC_MR_TRGSEL(value) ((DACC_MR_TRGSEL_Msk & ((value) << DACC_MR_TRGSEL_Pos)))
#define DACC_MR_WORD (0x1u << 4) /**< \brief (DACC_MR) Word Transfer */
#define   DACC_MR_WORD_HALF (0x0u << 4) /**< \brief (DACC_MR) Half-Word transfer */
#define   DACC_MR_WORD_WORD (0x1u << 4) /**< \brief (DACC_MR) Word Transfer */
#define DACC_MR_SLEEP (0x1u << 5) /**< \brief (DACC_MR) Sleep Mode */
#define DACC_MR_FASTWKUP (0x1u << 6) /**< \brief (DACC_MR) Fast Wake up Mode */
#define DACC_MR_ONE (0x1u << 8) /**< \brief (DACC_MR) Bit 8 must always be set to 1 when programming the DACC_MR */
#define DACC_MR_REFRESH_Pos 8
#define DACC_MR_REFRESH_Msk (0xffu << DACC_MR_REFRESH_Pos) /**< \brief (DACC_MR) Refresh Period */
#define DACC_MR_REFRESH(value) ((DACC_MR_REFRESH_Msk & ((value) << DACC_MR_REFRESH_Pos)))
#define DACC_MR_USER_SEL_Pos 16
#define DACC_MR_USER_SEL_Msk (0x3u << DACC_MR_USER_SEL_Pos) /**< \brief (DACC_MR) User Channel Selection */
#define   DACC_MR_USER_SEL_CHANNEL0 (0x0u << 16) /**< \brief (DACC_MR) Channel 0 */
#define   DACC_MR_USER_SEL_CHANNEL1 (0x1u << 16) /**< \brief (DACC_MR) Channel 1 */
#define DACC_MR_TAG (0x1u << 20) /**< \brief (DACC_MR) Tag Selection Mode */
#define   DACC_MR_TAG_DIS (0x0u << 20) /**< \brief (DACC_MR) Tag selection mode disabled. Using USER_SEL to select the channel for the conversion. */
#define   DACC_MR_TAG_EN (0x1u << 20) /**< \brief (DACC_MR) Tag selection mode enabled */
#define DACC_MR_MAXS (0x1u << 21) /**< \brief (DACC_MR) Max Speed Mode */
#define   DACC_MR_MAXS_NORMAL (0x0u << 21) /**< \brief (DACC_MR) Normal Mode */
#define   DACC_MR_MAXS_MAXIMUM (0x1u << 21) /**< \brief (DACC_MR) Max Speed Mode enabled */
#define DACC_MR_STARTUP_Pos 24
#define DACC_MR_STARTUP_Msk (0x3fu << DACC_MR_STARTUP_Pos) /**< \brief (DACC_MR) Startup Time Selection */
#define   DACC_MR_STARTUP_0 (0x0u << 24) /**< \brief (DACC_MR) 0 periods of DACClock */
#define   DACC_MR_STARTUP_8 (0x1u << 24) /**< \brief (DACC_MR) 8 periods of DACClock */
#define   DACC_MR_STARTUP_16 (0x2u << 24) /**< \brief (DACC_MR) 16 periods of DACClock */
#define   DACC_MR_STARTUP_24 (0x3u << 24) /**< \brief (DACC_MR) 24 periods of DACClock */
#define   DACC_MR_STARTUP_64 (0x4u << 24) /**< \brief (DACC_MR) 64 periods of DACClock */
#define   DACC_MR_STARTUP_80 (0x5u << 24) /**< \brief (DACC_MR) 80 periods of DACClock */
#define   DACC_MR_STARTUP_96 (0x6u << 24) /**< \brief (DACC_MR) 96 periods of DACClock */
#define   DACC_MR_STARTUP_112 (0x7u << 24) /**< \brief (DACC_MR) 112 periods of DACClock */
#define   DACC_MR_STARTUP_512 (0x8u << 24) /**< \brief (DACC_MR) 512 periods of DACClock */
#define   DACC_MR_STARTUP_576 (0x9u << 24) /**< \brief (DACC_MR) 576 periods of DACClock */
#define   DACC_MR_STARTUP_640 (0xAu << 24) /**< \brief (DACC_MR) 640 periods of DACClock */
#define   DACC_MR_STARTUP_704 (0xBu << 24) /**< \brief (DACC_MR) 704 periods of DACClock */
#define   DACC_MR_STARTUP_768 (0xCu << 24) /**< \brief (DACC_MR) 768 periods of DACClock */
#define   DACC_MR_STARTUP_832 (0xDu << 24) /**< \brief (DACC_MR) 832 periods of DACClock */
#define   DACC_MR_STARTUP_896 (0xEu << 24) /**< \brief (DACC_MR) 896 periods of DACClock */
#define   DACC_MR_STARTUP_960 (0xFu << 24) /**< \brief (DACC_MR) 960 periods of DACClock */
#define   DACC_MR_STARTUP_1024 (0x10u << 24) /**< \brief (DACC_MR) 1024 periods of DACClock */
#define   DACC_MR_STARTUP_1088 (0x11u << 24) /**< \brief (DACC_MR) 1088 periods of DACClock */
#define   DACC_MR_STARTUP_1152 (0x12u << 24) /**< \brief (DACC_MR) 1152 periods of DACClock */
#define   DACC_MR_STARTUP_1216 (0x13u << 24) /**< \brief (DACC_MR) 1216 periods of DACClock */
#define   DACC_MR_STARTUP_1280 (0x14u << 24) /**< \brief (DACC_MR) 1280 periods of DACClock */
#define   DACC_MR_STARTUP_1344 (0x15u << 24) /**< \brief (DACC_MR) 1344 periods of DACClock */
#define   DACC_MR_STARTUP_1408 (0x16u << 24) /**< \brief (DACC_MR) 1408 periods of DACClock */
#define   DACC_MR_STARTUP_1472 (0x17u << 24) /**< \brief (DACC_MR) 1472 periods of DACClock */
#define   DACC_MR_STARTUP_1536 (0x18u << 24) /**< \brief (DACC_MR) 1536 periods of DACClock */
#define   DACC_MR_STARTUP_1600 (0x19u << 24) /**< \brief (DACC_MR) 1600 periods of DACClock */
#define   DACC_MR_STARTUP_1664 (0x1Au << 24) /**< \brief (DACC_MR) 1664 periods of DACClock */
#define   DACC_MR_STARTUP_1728 (0x1Bu << 24) /**< \brief (DACC_MR) 1728 periods of DACClock */
#define   DACC_MR_STARTUP_1792 (0x1Cu << 24) /**< \brief (DACC_MR) 1792 periods of DACClock */
#define   DACC_MR_STARTUP_1856 (0x1Du << 24) /**< \brief (DACC_MR) 1856 periods of DACClock */
#define   DACC_MR_STARTUP_1920 (0x1Eu << 24) /**< \brief (DACC_MR) 1920 periods of DACClock */
#define   DACC_MR_STARTUP_1984 (0x1Fu << 24) /**< \brief (DACC_MR) 1984 periods of DACClock */
#define   DACC_MR_STARTUP_2048 (0x20u << 24) /**< \brief (DACC_MR) 2048 periods of DACClock */
#define   DACC_MR_STARTUP_2112 (0x21u << 24) /**< \brief (DACC_MR) 2112 periods of DACClock */
#define   DACC_MR_STARTUP_2176 (0x22u << 24) /**< \brief (DACC_MR) 2176 periods of DACClock */
#define   DACC_MR_STARTUP_2240 (0x23u << 24) /**< \brief (DACC_MR) 2240 periods of DACClock */
#define   DACC_MR_STARTUP_2304 (0x24u << 24) /**< \brief (DACC_MR) 2304 periods of DACClock */
#define   DACC_MR_STARTUP_2368 (0x25u << 24) /**< \brief (DACC_MR) 2368 periods of DACClock */
#define   DACC_MR_STARTUP_2432 (0x26u << 24) /**< \brief (DACC_MR) 2432 periods of DACClock */
#define   DACC_MR_STARTUP_2496 (0x27u << 24) /**< \brief (DACC_MR) 2496 periods of DACClock */
#define   DACC_MR_STARTUP_2560 (0x28u << 24) /**< \brief (DACC_MR) 2560 periods of DACClock */
#define   DACC_MR_STARTUP_2624 (0x29u << 24) /**< \brief (DACC_MR) 2624 periods of DACClock */
#define   DACC_MR_STARTUP_2688 (0x2Au << 24) /**< \brief (DACC_MR) 2688 periods of DACClock */
#define   DACC_MR_STARTUP_2752 (0x2Bu << 24) /**< \brief (DACC_MR) 2752 periods of DACClock */
#define   DACC_MR_STARTUP_2816 (0x2Cu << 24) /**< \brief (DACC_MR) 2816 periods of DACClock */
#define   DACC_MR_STARTUP_2880 (0x2Du << 24) /**< \brief (DACC_MR) 2880 periods of DACClock */
#define   DACC_MR_STARTUP_2944 (0x2Eu << 24) /**< \brief (DACC_MR) 2944 periods of DACClock */
#define   DACC_MR_STARTUP_3008 (0x2Fu << 24) /**< \brief (DACC_MR) 3008 periods of DACClock */
#define   DACC_MR_STARTUP_3072 (0x30u << 24) /**< \brief (DACC_MR) 3072 periods of DACClock */
#define   DACC_MR_STARTUP_3136 (0x31u << 24) /**< \brief (DACC_MR) 3136 periods of DACClock */
#define   DACC_MR_STARTUP_3200 (0x32u << 24) /**< \brief (DACC_MR) 3200 periods of DACClock */
#define   DACC_MR_STARTUP_3264 (0x33u << 24) /**< \brief (DACC_MR) 3264 periods of DACClock */
#define   DACC_MR_STARTUP_3328 (0x34u << 24) /**< \brief (DACC_MR) 3328 periods of DACClock */
#define   DACC_MR_STARTUP_3392 (0x35u << 24) /**< \brief (DACC_MR) 3392 periods of DACClock */
#define   DACC_MR_STARTUP_3456 (0x36u << 24) /**< \brief (DACC_MR) 3456 periods of DACClock */
#define   DACC_MR_STARTUP_3520 (0x37u << 24) /**< \brief (DACC_MR) 3520 periods of DACClock */
#define   DACC_MR_STARTUP_3584 (0x38u << 24) /**< \brief (DACC_MR) 3584 periods of DACClock */
#define   DACC_MR_STARTUP_3648 (0x39u << 24) /**< \brief (DACC_MR) 3648 periods of DACClock */
#define   DACC_MR_STARTUP_3712 (0x3Au << 24) /**< \brief (DACC_MR) 3712 periods of DACClock */
#define   DACC_MR_STARTUP_3776 (0x3Bu << 24) /**< \brief (DACC_MR) 3776 periods of DACClock */
#define   DACC_MR_STARTUP_3840 (0x3Cu << 24) /**< \brief (DACC_MR) 3840 periods of DACClock */
#define   DACC_MR_STARTUP_3904 (0x3Du << 24) /**< \brief (DACC_MR) 3904 periods of DACClock */
#define   DACC_MR_STARTUP_3968 (0x3Eu << 24) /**< \brief (DACC_MR) 3968 periods of DACClock */
#define   DACC_MR_STARTUP_4032 (0x3Fu << 24) /**< \brief (DACC_MR) 4032 periods of DACClock */
/* -------- DACC_CHER : (DACC Offset: 0x10) Channel Enable Register -------- */
#define DACC_CHER_CH0 (0x1u << 0) /**< \brief (DACC_CHER) Channel 0 Enable */
#define DACC_CHER_CH1 (0x1u << 1) /**< \brief (DACC_CHER) Channel 1 Enable */
/* -------- DACC_CHDR : (DACC Offset: 0x14) Channel Disable Register -------- */
#define DACC_CHDR_CH0 (0x1u << 0) /**< \brief (DACC_CHDR) Channel 0 Disable */
#define DACC_CHDR_CH1 (0x1u << 1) /**< \brief (DACC_CHDR) Channel 1 Disable */
/* -------- DACC_CHSR : (DACC Offset: 0x18) Channel Status Register -------- */
#define DACC_CHSR_CH0 (0x1u << 0) /**< \brief (DACC_CHSR) Channel 0 Status */
#define DACC_CHSR_CH1 (0x1u << 1) /**< \brief (DACC_CHSR) Channel 1 Status */
/* -------- DACC_CDR : (DACC Offset: 0x20) Conversion Data Register -------- */
#define DACC_CDR_DATA_Pos 0
#define DACC_CDR_DATA_Msk (0xffffffffu << DACC_CDR_DATA_Pos) /**< \brief (DACC_CDR) Data to Convert */
#define DACC_CDR_DATA(value) ((DACC_CDR_DATA_Msk & ((value) << DACC_CDR_DATA_Pos)))
/* -------- DACC_IER : (DACC Offset: 0x24) Interrupt Enable Register -------- */
#define DACC_IER_TXRDY (0x1u << 0) /**< \brief (DACC_IER) Transmit Ready Interrupt Enable */
#define DACC_IER_EOC (0x1u << 1) /**< \brief (DACC_IER) End of Conversion Interrupt Enable */
#define DACC_IER_ENDTX (0x1u << 2) /**< \brief (DACC_IER) End of Transmit Buffer Interrupt Enable */
#define DACC_IER_TXBUFE (0x1u << 3) /**< \brief (DACC_IER) Transmit Buffer Empty Interrupt Enable */
/* -------- DACC_IDR : (DACC Offset: 0x28) Interrupt Disable Register -------- */
#define DACC_IDR_TXRDY (0x1u << 0) /**< \brief (DACC_IDR) Transmit Ready Interrupt Disable. */
#define DACC_IDR_EOC (0x1u << 1) /**< \brief (DACC_IDR) End of Conversion Interrupt Disable */
#define DACC_IDR_ENDTX (0x1u << 2) /**< \brief (DACC_IDR) End of Transmit Buffer Interrupt Disable */
#define DACC_IDR_TXBUFE (0x1u << 3) /**< \brief (DACC_IDR) Transmit Buffer Empty Interrupt Disable */
/* -------- DACC_IMR : (DACC Offset: 0x2C) Interrupt Mask Register -------- */
#define DACC_IMR_TXRDY (0x1u << 0) /**< \brief (DACC_IMR) Transmit Ready Interrupt Mask */
#define DACC_IMR_EOC (0x1u << 1) /**< \brief (DACC_IMR) End of Conversion Interrupt Mask */
#define DACC_IMR_ENDTX (0x1u << 2) /**< \brief (DACC_IMR) End of Transmit Buffer Interrupt Mask */
#define DACC_IMR_TXBUFE (0x1u << 3) /**< \brief (DACC_IMR) Transmit Buffer Empty Interrupt Mask */
/* -------- DACC_ISR : (DACC Offset: 0x30) Interrupt Status Register -------- */
#define DACC_ISR_TXRDY (0x1u << 0) /**< \brief (DACC_ISR) Transmit Ready Interrupt Flag */
#define DACC_ISR_EOC (0x1u << 1) /**< \brief (DACC_ISR) End of Conversion Interrupt Flag */
#define DACC_ISR_ENDTX (0x1u << 2) /**< \brief (DACC_ISR) End of DMA Interrupt Flag */
#define DACC_ISR_TXBUFE (0x1u << 3) /**< \brief (DACC_ISR) Transmit Buffer Empty */
/* -------- DACC_ACR : (DACC Offset: 0x94) Analog Current Register -------- */
#define DACC_ACR_IBCTLCH0_Pos 0
#define DACC_ACR_IBCTLCH0_Msk (0x3u << DACC_ACR_IBCTLCH0_Pos) /**< \brief (DACC_ACR) Analog Output Current Control */
#define DACC_ACR_IBCTLCH0(value) ((DACC_ACR_IBCTLCH0_Msk & ((value) << DACC_ACR_IBCTLCH0_Pos)))
#define DACC_ACR_IBCTLCH1_Pos 2
#define DACC_ACR_IBCTLCH1_Msk (0x3u << DACC_ACR_IBCTLCH1_Pos) /**< \brief (DACC_ACR) Analog Output Current Control */
#define DACC_ACR_IBCTLCH1(value) ((DACC_ACR_IBCTLCH1_Msk & ((value) << DACC_ACR_IBCTLCH1_Pos)))
#define DACC_ACR_IBCTLDACCORE_Pos 8
#define DACC_ACR_IBCTLDACCORE_Msk (0x3u << DACC_ACR_IBCTLDACCORE_Pos) /**< \brief (DACC_ACR) Bias Current Control for DAC Core */
#define DACC_ACR_IBCTLDACCORE(value) ((DACC_ACR_IBCTLDACCORE_Msk & ((value) << DACC_ACR_IBCTLDACCORE_Pos)))
/* -------- DACC_WPMR : (DACC Offset: 0xE4) Write Protect Mode register -------- */
#define DACC_WPMR_WPEN (0x1u << 0) /**< \brief (DACC_WPMR) Write Protect Enable */
#define DACC_WPMR_WPKEY_Pos 8
#define DACC_WPMR_WPKEY_Msk (0xffffffu << DACC_WPMR_WPKEY_Pos) /**< \brief (DACC_WPMR) Write Protect KEY */
#define DACC_WPMR_WPKEY(value) ((DACC_WPMR_WPKEY_Msk & ((value) << DACC_WPMR_WPKEY_Pos)))
/* -------- DACC_WPSR : (DACC Offset: 0xE8) Write Protect Status register -------- */
#define DACC_WPSR_WPROTERR (0x1u << 0) /**< \brief (DACC_WPSR) Write protection error */
#define DACC_WPSR_WPROTADDR_Pos 8
#define DACC_WPSR_WPROTADDR_Msk (0xffu << DACC_WPSR_WPROTADDR_Pos) /**< \brief (DACC_WPSR) Write protection error address */
/* -------- DACC_TPR : (DACC Offset: 0x108) Transmit Pointer Register -------- */
#define DACC_TPR_TXPTR_Pos 0
#define DACC_TPR_TXPTR_Msk (0xffffffffu << DACC_TPR_TXPTR_Pos) /**< \brief (DACC_TPR) Transmit Counter Register */
#define DACC_TPR_TXPTR(value) ((DACC_TPR_TXPTR_Msk & ((value) << DACC_TPR_TXPTR_Pos)))
/* -------- DACC_TCR : (DACC Offset: 0x10C) Transmit Counter Register -------- */
#define DACC_TCR_TXCTR_Pos 0
#define DACC_TCR_TXCTR_Msk (0xffffu << DACC_TCR_TXCTR_Pos) /**< \brief (DACC_TCR) Transmit Counter Register */
#define DACC_TCR_TXCTR(value) ((DACC_TCR_TXCTR_Msk & ((value) << DACC_TCR_TXCTR_Pos)))
/* -------- DACC_TNPR : (DACC Offset: 0x118) Transmit Next Pointer Register -------- */
#define DACC_TNPR_TXNPTR_Pos 0
#define DACC_TNPR_TXNPTR_Msk (0xffffffffu << DACC_TNPR_TXNPTR_Pos) /**< \brief (DACC_TNPR) Transmit Next Pointer */
#define DACC_TNPR_TXNPTR(value) ((DACC_TNPR_TXNPTR_Msk & ((value) << DACC_TNPR_TXNPTR_Pos)))
/* -------- DACC_TNCR : (DACC Offset: 0x11C) Transmit Next Counter Register -------- */
#define DACC_TNCR_TXNCTR_Pos 0
#define DACC_TNCR_TXNCTR_Msk (0xffffu << DACC_TNCR_TXNCTR_Pos) /**< \brief (DACC_TNCR) Transmit Counter Next */
#define DACC_TNCR_TXNCTR(value) ((DACC_TNCR_TXNCTR_Msk & ((value) << DACC_TNCR_TXNCTR_Pos)))
/* -------- DACC_PTCR : (DACC Offset: 0x120) Transfer Control Register -------- */
#define DACC_PTCR_RXTEN (0x1u << 0) /**< \brief (DACC_PTCR) Receiver Transfer Enable */
#define DACC_PTCR_RXTDIS (0x1u << 1) /**< \brief (DACC_PTCR) Receiver Transfer Disable */
#define DACC_PTCR_TXTEN (0x1u << 8) /**< \brief (DACC_PTCR) Transmitter Transfer Enable */
#define DACC_PTCR_TXTDIS (0x1u << 9) /**< \brief (DACC_PTCR) Transmitter Transfer Disable */
/* -------- DACC_PTSR : (DACC Offset: 0x124) Transfer Status Register -------- */
#define DACC_PTSR_RXTEN (0x1u << 0) /**< \brief (DACC_PTSR) Receiver Transfer Enable */
#define DACC_PTSR_TXTEN (0x1u << 8) /**< \brief (DACC_PTSR) Transmitter Transfer Enable */

/*@}*/


#endif /* _SAM4S_DACC_COMPONENT_ */
