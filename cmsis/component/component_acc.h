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

#ifndef _SAM4S_ACC_COMPONENT_
#define _SAM4S_ACC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Analog Comparator Controller */
/* ============================================================================= */
/** \addtogroup SAM4S_ACC Analog Comparator Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Acc hardware registers */
typedef struct {
    __O  uint32_t ACC_CR;        /**< \brief (Acc Offset: 0x00) Control Register */
    __IO uint32_t ACC_MR;        /**< \brief (Acc Offset: 0x04) Mode Register */
    __I  uint32_t Reserved1[7];
    __O  uint32_t
    ACC_IER;       /**< \brief (Acc Offset: 0x24) Interrupt Enable Register */
    __O  uint32_t
    ACC_IDR;       /**< \brief (Acc Offset: 0x28) Interrupt Disable Register */
    __I  uint32_t
    ACC_IMR;       /**< \brief (Acc Offset: 0x2C) Interrupt Mask Register */
    __I  uint32_t
    ACC_ISR;       /**< \brief (Acc Offset: 0x30) Interrupt Status Register */
    __I  uint32_t Reserved2[24];
    __IO uint32_t
    ACC_ACR;       /**< \brief (Acc Offset: 0x94) Analog Control Register */
    __I  uint32_t Reserved3[19];
    __IO uint32_t
    ACC_WPMR;      /**< \brief (Acc Offset: 0xE4) Write Protect Mode Register */
    __I  uint32_t
    ACC_WPSR;      /**< \brief (Acc Offset: 0xE8) Write Protect Status Register */
} Acc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- ACC_CR : (ACC Offset: 0x00) Control Register -------- */
#define ACC_CR_SWRST (0x1u << 0) /**< \brief (ACC_CR) SoftWare ReSeT */
/* -------- ACC_MR : (ACC Offset: 0x04) Mode Register -------- */
#define ACC_MR_SELMINUS_Pos 0
#define ACC_MR_SELMINUS_Msk (0x7u << ACC_MR_SELMINUS_Pos) /**< \brief (ACC_MR) SELection for MINUS comparator input */
#define   ACC_MR_SELMINUS_TS (0x0u << 0) /**< \brief (ACC_MR) SelectTS */
#define   ACC_MR_SELMINUS_ADVREF (0x1u << 0) /**< \brief (ACC_MR) Select ADVREF */
#define   ACC_MR_SELMINUS_DAC0 (0x2u << 0) /**< \brief (ACC_MR) Select DAC0 */
#define   ACC_MR_SELMINUS_DAC1 (0x3u << 0) /**< \brief (ACC_MR) Select DAC1 */
#define   ACC_MR_SELMINUS_AD0 (0x4u << 0) /**< \brief (ACC_MR) Select AD0 */
#define   ACC_MR_SELMINUS_AD1 (0x5u << 0) /**< \brief (ACC_MR) Select AD1 */
#define   ACC_MR_SELMINUS_AD2 (0x6u << 0) /**< \brief (ACC_MR) Select AD2 */
#define   ACC_MR_SELMINUS_AD3 (0x7u << 0) /**< \brief (ACC_MR) Select AD3 */
#define ACC_MR_SELPLUS_Pos 4
#define ACC_MR_SELPLUS_Msk (0x7u << ACC_MR_SELPLUS_Pos) /**< \brief (ACC_MR) SELection for PLUS comparator input */
#define   ACC_MR_SELPLUS_AD0 (0x0u << 4) /**< \brief (ACC_MR) Select AD0 */
#define   ACC_MR_SELPLUS_AD1 (0x1u << 4) /**< \brief (ACC_MR) Select AD1 */
#define   ACC_MR_SELPLUS_AD2 (0x2u << 4) /**< \brief (ACC_MR) Select AD2 */
#define   ACC_MR_SELPLUS_AD3 (0x3u << 4) /**< \brief (ACC_MR) Select AD3 */
#define   ACC_MR_SELPLUS_AD4 (0x4u << 4) /**< \brief (ACC_MR) Select AD4 */
#define   ACC_MR_SELPLUS_AD5 (0x5u << 4) /**< \brief (ACC_MR) Select AD5 */
#define   ACC_MR_SELPLUS_AD6 (0x6u << 4) /**< \brief (ACC_MR) Select AD6 */
#define   ACC_MR_SELPLUS_AD7 (0x7u << 4) /**< \brief (ACC_MR) Select AD7 */
#define ACC_MR_ACEN (0x1u << 8) /**< \brief (ACC_MR) Analog Comparator ENable */
#define   ACC_MR_ACEN_DIS (0x0u << 8) /**< \brief (ACC_MR) Analog Comparator Disabled. */
#define   ACC_MR_ACEN_EN (0x1u << 8) /**< \brief (ACC_MR) Analog Comparator Enabled. */
#define ACC_MR_EDGETYP_Pos 9
#define ACC_MR_EDGETYP_Msk (0x3u << ACC_MR_EDGETYP_Pos) /**< \brief (ACC_MR) EDGE TYPe */
#define   ACC_MR_EDGETYP_RISING (0x0u << 9) /**< \brief (ACC_MR) only rising edge of comparator output */
#define   ACC_MR_EDGETYP_FALLING (0x1u << 9) /**< \brief (ACC_MR) falling edge of comparator output */
#define   ACC_MR_EDGETYP_ANY (0x2u << 9) /**< \brief (ACC_MR) any edge of comparator output */
#define ACC_MR_INV (0x1u << 12) /**< \brief (ACC_MR) INVert comparator output */
#define   ACC_MR_INV_DIS (0x0u << 12) /**< \brief (ACC_MR) Analog Comparator output is directly processed. */
#define   ACC_MR_INV_EN (0x1u << 12) /**< \brief (ACC_MR) Analog Comparator output is inverted prior to being processed. */
#define ACC_MR_SELFS (0x1u << 13) /**< \brief (ACC_MR) SELection of Fault Source */
#define   ACC_MR_SELFS_CF (0x0u << 13) /**< \brief (ACC_MR) the CF flag is used to drive the FAULT output. */
#define   ACC_MR_SELFS_OUTPUT (0x1u << 13) /**< \brief (ACC_MR) the output of the Analog Comparator flag is used to drive the FAULT output. */
#define ACC_MR_FE (0x1u << 14) /**< \brief (ACC_MR) Fault Enable */
#define   ACC_MR_FE_DIS (0x0u << 14) /**< \brief (ACC_MR) the FAULT output is tied to 0. */
#define   ACC_MR_FE_EN (0x1u << 14) /**< \brief (ACC_MR) the FAULT output is driven by the signal defined by SELFS. */
/* -------- ACC_IER : (ACC Offset: 0x24) Interrupt Enable Register -------- */
#define ACC_IER_CE (0x1u << 0) /**< \brief (ACC_IER) Comparison Edge */
/* -------- ACC_IDR : (ACC Offset: 0x28) Interrupt Disable Register -------- */
#define ACC_IDR_CE (0x1u << 0) /**< \brief (ACC_IDR) Comparison Edge */
/* -------- ACC_IMR : (ACC Offset: 0x2C) Interrupt Mask Register -------- */
#define ACC_IMR_CE (0x1u << 0) /**< \brief (ACC_IMR) Comparison Edge */
/* -------- ACC_ISR : (ACC Offset: 0x30) Interrupt Status Register -------- */
#define ACC_ISR_CE (0x1u << 0) /**< \brief (ACC_ISR) Comparison Edge */
#define ACC_ISR_SCO (0x1u << 1) /**< \brief (ACC_ISR) Synchronized Comparator Output */
#define ACC_ISR_MASK (0x1u << 31) /**< \brief (ACC_ISR) Flag Mask */
/* -------- ACC_ACR : (ACC Offset: 0x94) Analog Control Register -------- */
#define ACC_ACR_ISEL (0x1u << 0) /**< \brief (ACC_ACR) Current SELection */
#define   ACC_ACR_ISEL_LOPW (0x0u << 0) /**< \brief (ACC_ACR) low power option. */
#define   ACC_ACR_ISEL_HISP (0x1u << 0) /**< \brief (ACC_ACR) high speed option. */
#define ACC_ACR_HYST_Pos 1
#define ACC_ACR_HYST_Msk (0x3u << ACC_ACR_HYST_Pos) /**< \brief (ACC_ACR) HYSTeresis selection */
#define ACC_ACR_HYST(value) ((ACC_ACR_HYST_Msk & ((value) << ACC_ACR_HYST_Pos)))
/* -------- ACC_WPMR : (ACC Offset: 0xE4) Write Protect Mode Register -------- */
#define ACC_WPMR_WPEN (0x1u << 0) /**< \brief (ACC_WPMR) Write Protect Enable */
#define ACC_WPMR_WPKEY_Pos 8
#define ACC_WPMR_WPKEY_Msk (0xffffffu << ACC_WPMR_WPKEY_Pos) /**< \brief (ACC_WPMR) Write Protect KEY */
#define   ACC_WPMR_WPKEY_PASSWD (0x414343u << 8) /**< \brief (ACC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. */
/* -------- ACC_WPSR : (ACC Offset: 0xE8) Write Protect Status Register -------- */
#define ACC_WPSR_WPROTERR (0x1u << 0) /**< \brief (ACC_WPSR) Write PROTection ERRor */

/*@}*/


#endif /* _SAM4S_ACC_COMPONENT_ */
