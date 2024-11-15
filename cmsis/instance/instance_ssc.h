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

#ifndef _SAM4S_SSC_INSTANCE_
#define _SAM4S_SSC_INSTANCE_

/* ========== Register definition for SSC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
    #define REG_SSC_CR                    (0x40004000U) /**< \brief (SSC) Control Register */
    #define REG_SSC_CMR                   (0x40004004U) /**< \brief (SSC) Clock Mode Register */
    #define REG_SSC_RCMR                  (0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
    #define REG_SSC_RFMR                  (0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
    #define REG_SSC_TCMR                  (0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
    #define REG_SSC_TFMR                  (0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
    #define REG_SSC_RHR                   (0x40004020U) /**< \brief (SSC) Receive Holding Register */
    #define REG_SSC_THR                   (0x40004024U) /**< \brief (SSC) Transmit Holding Register */
    #define REG_SSC_RSHR                  (0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
    #define REG_SSC_TSHR                  (0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
    #define REG_SSC_RC0R                  (0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
    #define REG_SSC_RC1R                  (0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
    #define REG_SSC_SR                    (0x40004040U) /**< \brief (SSC) Status Register */
    #define REG_SSC_IER                   (0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
    #define REG_SSC_IDR                   (0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
    #define REG_SSC_IMR                   (0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
    #define REG_SSC_WPMR                  (0x400040E4U) /**< \brief (SSC) Write Protect Mode Register */
    #define REG_SSC_WPSR                  (0x400040E8U) /**< \brief (SSC) Write Protect Status Register */
    #define REG_SSC_RPR                   (0x40004100U) /**< \brief (SSC) Receive Pointer Register */
    #define REG_SSC_RCR                   (0x40004104U) /**< \brief (SSC) Receive Counter Register */
    #define REG_SSC_TPR                   (0x40004108U) /**< \brief (SSC) Transmit Pointer Register */
    #define REG_SSC_TCR                   (0x4000410CU) /**< \brief (SSC) Transmit Counter Register */
    #define REG_SSC_RNPR                  (0x40004110U) /**< \brief (SSC) Receive Next Pointer Register */
    #define REG_SSC_RNCR                  (0x40004114U) /**< \brief (SSC) Receive Next Counter Register */
    #define REG_SSC_TNPR                  (0x40004118U) /**< \brief (SSC) Transmit Next Pointer Register */
    #define REG_SSC_TNCR                  (0x4000411CU) /**< \brief (SSC) Transmit Next Counter Register */
    #define REG_SSC_PTCR                  (0x40004120U) /**< \brief (SSC) Transfer Control Register */
    #define REG_SSC_PTSR                  (0x40004124U) /**< \brief (SSC) Transfer Status Register */
#else
    #define REG_SSC_CR   (*(__O  uint32_t*)0x40004000U) /**< \brief (SSC) Control Register */
    #define REG_SSC_CMR  (*(__IO uint32_t*)0x40004004U) /**< \brief (SSC) Clock Mode Register */
    #define REG_SSC_RCMR (*(__IO uint32_t*)0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
    #define REG_SSC_RFMR (*(__IO uint32_t*)0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
    #define REG_SSC_TCMR (*(__IO uint32_t*)0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
    #define REG_SSC_TFMR (*(__IO uint32_t*)0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
    #define REG_SSC_RHR  (*(__I  uint32_t*)0x40004020U) /**< \brief (SSC) Receive Holding Register */
    #define REG_SSC_THR  (*(__O  uint32_t*)0x40004024U) /**< \brief (SSC) Transmit Holding Register */
    #define REG_SSC_RSHR (*(__I  uint32_t*)0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
    #define REG_SSC_TSHR (*(__IO uint32_t*)0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
    #define REG_SSC_RC0R (*(__IO uint32_t*)0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
    #define REG_SSC_RC1R (*(__IO uint32_t*)0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
    #define REG_SSC_SR   (*(__I  uint32_t*)0x40004040U) /**< \brief (SSC) Status Register */
    #define REG_SSC_IER  (*(__O  uint32_t*)0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
    #define REG_SSC_IDR  (*(__O  uint32_t*)0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
    #define REG_SSC_IMR  (*(__I  uint32_t*)0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
    #define REG_SSC_WPMR (*(__IO uint32_t*)0x400040E4U) /**< \brief (SSC) Write Protect Mode Register */
    #define REG_SSC_WPSR (*(__I  uint32_t*)0x400040E8U) /**< \brief (SSC) Write Protect Status Register */
    #define REG_SSC_RPR  (*(__IO uint32_t*)0x40004100U) /**< \brief (SSC) Receive Pointer Register */
    #define REG_SSC_RCR  (*(__IO uint32_t*)0x40004104U) /**< \brief (SSC) Receive Counter Register */
    #define REG_SSC_TPR  (*(__IO uint32_t*)0x40004108U) /**< \brief (SSC) Transmit Pointer Register */
    #define REG_SSC_TCR  (*(__IO uint32_t*)0x4000410CU) /**< \brief (SSC) Transmit Counter Register */
    #define REG_SSC_RNPR (*(__IO uint32_t*)0x40004110U) /**< \brief (SSC) Receive Next Pointer Register */
    #define REG_SSC_RNCR (*(__IO uint32_t*)0x40004114U) /**< \brief (SSC) Receive Next Counter Register */
    #define REG_SSC_TNPR (*(__IO uint32_t*)0x40004118U) /**< \brief (SSC) Transmit Next Pointer Register */
    #define REG_SSC_TNCR (*(__IO uint32_t*)0x4000411CU) /**< \brief (SSC) Transmit Next Counter Register */
    #define REG_SSC_PTCR (*(__O  uint32_t*)0x40004120U) /**< \brief (SSC) Transfer Control Register */
    #define REG_SSC_PTSR (*(__I  uint32_t*)0x40004124U) /**< \brief (SSC) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4S_SSC_INSTANCE_ */
