/**
 * \file
 *
 * \brief Common SD/MMC stack
 *
 * Copyright (c) 2012-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "sam4lc4c.h"
#include <string.h>
#include "sd_mmc_protocol.h"
#include "sd_mmc.h"
#include "ctrl_access.h"
#include "ff.h"
#include "debug.h"
#include "delays.h"

#define SD_MMC_SPI_MEM_CNT      1
#define PIN_SD_DETECTION    13
#define PORT_SD_DETECTION   1

#define SD_MMC_0_CD_DETECT_VALUE    0

/**
 * \ingroup sd_mmc_stack
 * \defgroup sd_mmc_stack_internal Implementation of SD/MMC/SDIO Stack
 * @{
 */
#    include "sd_mmc_spi.h"
#    define sd_mmc_spi  sd_mmc_spi
#    define SD_MMC_MEM_CNT        SD_MMC_SPI_MEM_CNT
#    define sd_mmc_is_spi()       true

#define sd_mmc_is_mci()  (!sd_mmc_is_spi())

//! This SD MMC stack supports only the high voltage
#define SD_MMC_VOLTAGE_SUPPORT \
    (OCR_VDD_27_28 | OCR_VDD_28_29 | \
     OCR_VDD_29_30 | OCR_VDD_30_31 | \
     OCR_VDD_31_32 | OCR_VDD_32_33)

//! SD/MMC card states
enum card_state {
    SD_MMC_CARD_STATE_READY = 0,          //!< Ready to use
    SD_MMC_CARD_STATE_DEBOUNCE = 1,          //!< Debounce on going
    SD_MMC_CARD_STATE_INIT = 2,          //!< Initialization on going
    SD_MMC_CARD_STATE_UNUSABLE = 3,          //!< Unusable card
    SD_MMC_CARD_STATE_NO_CARD = 4,          //!< No SD/MMC card inserted
};

//! SD/MMC card information structure
struct sd_mmc_card {
    uint32_t clock;          //!< Card access clock
    uint32_t capacity;          //!< Card capacity in KBytes
    uint32_t cd_gpio;          //!< Card detect GPIO
    uint16_t rca;          //!< Relative card address
    enum card_state state;          //!< Card state
    card_type_t type;          //!< Card type
    card_version_t version;          //!< Card version
    uint8_t bus_width;          //!< Number of DATA lin on bus (MCI only)
    uint8_t csd[CSD_REG_BSIZE];          //!< CSD register
    uint8_t high_speed;          //!< High speed card (1)
};

_STATIC struct sd_mmc_card sd_mmc_cards[SD_MMC_MEM_CNT];

//! Index of current slot configurated
_STATIC uint8_t sd_mmc_slot_sel;
//! Pointer on current slot configurated
_STATIC struct sd_mmc_card* sd_mmc_card;
//! Number of block to read or write on the current transfer
_STATIC uint16_t sd_mmc_nb_block_to_tranfer = 0;
//! Number of block remaining to read or write on the current transfer
_STATIC uint16_t sd_mmc_nb_block_remaining = 0;

//! SD/MMC transfer rate unit codes (10K) list
const uint32_t sd_mmc_trans_units[7] = { 10, 100, 1000, 10000, 0, 0, 0 };
//! SD transfer multiplier factor codes (1/10) list
const uint32_t sd_trans_multipliers[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35,
                                            40, 45, 50, 55, 60, 70, 80
                                          };
//! MMC transfer multiplier factor codes (1/10) list
const uint32_t mmc_trans_multipliers[16] = { 0, 10, 12, 13, 15, 20, 26, 30, 35,
                                             40, 45, 52, 55, 60, 70, 80
                                           };

//! \name MMC, SD and SDIO commands process
//! @{
_STATIC bool mmc_spi_op_cond(void);
_STATIC bool mmc_mci_op_cond(void);
_STATIC bool sd_spi_op_cond(uint8_t v2);
_STATIC bool sd_mci_op_cond(uint8_t v2);
_STATIC bool sdio_op_cond(void);
_STATIC bool sdio_get_max_speed(void);
_STATIC bool sdio_cmd52_set_bus_width(void);
_STATIC bool sdio_cmd52_set_high_speed(void);
_STATIC bool sd_cm6_set_high_speed(void);
_STATIC bool mmc_cmd6_set_bus_width(uint8_t bus_width);
_STATIC bool mmc_cmd6_set_high_speed(void);
_STATIC bool sd_cmd8(uint8_t* v2);
_STATIC bool mmc_cmd8(uint8_t* b_authorize_high_speed);
_STATIC bool sd_mmc_cmd9_spi(void);
_STATIC bool sd_mmc_cmd9_mci(void);
_STATIC void mmc_decode_csd(void);
_STATIC void sd_decode_csd(void);
_STATIC bool sd_mmc_cmd13(void);
_STATIC bool sd_acmd6(void);
_STATIC bool sd_acmd51(void);
//! @}

//! \name Internal function to process the initialization and install
//! @{
_STATIC sd_mmc_err_t sd_mmc_select_slot(uint8_t slot);
_STATIC void sd_mmc_configure_slot(void);
_STATIC void sd_mmc_deselect_slot(void);
_STATIC bool sd_mmc_spi_card_init(void);
_STATIC bool sd_mmc_mci_card_init(void);
_STATIC bool sd_mmc_spi_install_mmc(void);
_STATIC bool sd_mmc_mci_install_mmc(void);
//! @}

//! \name Internal functions to manage a large timeout after a card insertion
//! @{
#define SD_MMC_DEBOUNCE_TIMEOUT   1000 // Unit ms

_STATIC bool sd_mmc_sam_systick_used;

_STATIC inline void SD_MMC_START_TIMEOUT(void)
{
    if (!SysTick->CTRL) {
        sd_mmc_sam_systick_used = true;
        SysTick->LOAD = (CLOCK_HZ / (8 * 1000)) * SD_MMC_DEBOUNCE_TIMEOUT;
        SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
    } else {
        sd_mmc_sam_systick_used = false;
        delay_ms(SD_MMC_DEBOUNCE_TIMEOUT);

    }
}

_STATIC inline bool SD_MMC_IS_TIMEOUT(void)
{
    if (!sd_mmc_sam_systick_used) {
        return true;
    }

    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
        SysTick->CTRL = 0;
        return true;
    }

    return false;
}

_STATIC inline void SD_MMC_STOP_TIMEOUT(void)
{
    if (sd_mmc_sam_systick_used) {
        SysTick->CTRL = 0;
    }
}
//! @}

/**
 * \brief Sends operation condition command and read OCR (SPI only)
 * - CMD1 sends operation condition command
 * - CMD58 reads OCR
 *
 * \return true if success, otherwise false
 */
_STATIC bool mmc_spi_op_cond(void)
{
    uint32_t retry, resp;

    /*
     * Timeout 1s = 400KHz / ((6+1)*8) cylces = 7150 retry
     * 6 = cmd byte size
     * 1 = response byte size
     */
    retry = 7150;

    do {
        if (!sd_mmc_spi_send_cmd(MMC_SPI_CMD1_SEND_OP_COND, 0)) {
            return false;
        }

        // Check busy flag
        resp = sd_mmc_spi_get_response();

        if (!(resp & R1_SPI_IDLE)) {
            break;
        }

        if (retry-- == 0) {
            return false;
        }
    } while (1);

    // Read OCR for SPI mode
    if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD58_READ_OCR, 0)) {
        return false;
    }

    // Check OCR value
    if ((sd_mmc_spi_get_response() & OCR_ACCESS_MODE_MASK)
        == OCR_ACCESS_MODE_SECTOR) {
        sd_mmc_card->type |= CARD_TYPE_HC;
    }

    return true;
}

/**
 * \brief Sends operation condition command and read OCR (MCI only)
 * - CMD1 sends operation condition command
 * - CMD1 reads OCR
 *
 * \return true if success, otherwise false
 */
_STATIC bool mmc_mci_op_cond(void)
{
    uint32_t retry, resp;

    /*
     * Timeout 1s = 400KHz / ((6+6)*8) cylces = 4200 retry
     * 6 = cmd byte size
     * 6 = response byte size
     */
    retry = 4200;

    do {
        if (!sd_mmc_spi_send_cmd(MMC_MCI_CMD1_SEND_OP_COND,
                                 SD_MMC_VOLTAGE_SUPPORT | OCR_ACCESS_MODE_SECTOR)) {
            return false;
        }

        // Check busy flag
        resp = sd_mmc_spi_get_response();

        if (resp & OCR_POWER_UP_BUSY) {
            // Check OCR value
            if ((resp & OCR_ACCESS_MODE_MASK) == OCR_ACCESS_MODE_SECTOR) {
                sd_mmc_card->type |= CARD_TYPE_HC;
            }

            break;
        }

        if (retry-- == 0) {
            return false;
        }
    } while (1);

    return true;
}

/**
 * \brief Ask to all cards to send their operations conditions (SPI only).
 * - ACMD41 sends operation condition command.
 * - CMD58 reads OCR
 *
 * \param v2   Shall be 1 if it is a SD card V2
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_spi_op_cond(uint8_t v2)
{
    uint32_t arg, retry, resp;

    /*
     * Timeout 1s = 400KHz / ((6+1)*8) cylces = 7150 retry
     * 6 = cmd byte size
     * 1 = response byte size
     */
    retry = 7150;

    do {
        // CMD55 - Indicate to the card that the next command is an
        // application specific command rather than a standard command.
        if (!sd_mmc_spi_send_cmd(SDMMC_CMD55_APP_CMD, 0)) {
            return false;
        }

        // (ACMD41) Sends host OCR register
        arg = 0;

        if (v2) {
            arg |= SD_ACMD41_HCS;
        }

        // Check response
        if (!sd_mmc_spi_send_cmd(SD_SPI_ACMD41_SD_SEND_OP_COND, arg)) {
            return false;
        }

        resp = sd_mmc_spi_get_response();

        if (!(resp & R1_SPI_IDLE)) {
            // Card is ready
            break;
        }

        if (retry-- == 0) {
            return false;
        }
    } while (1);

    // Read OCR for SPI mode
    if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD58_READ_OCR, 0)) {
        return false;
    }

    if ((sd_mmc_spi_get_response() & OCR_CCS) != 0) {
        sd_mmc_card->type |= CARD_TYPE_HC;
    }

    return true;
}

/**
 * \brief Ask to all cards to send their operations conditions (MCI only).
 * - ACMD41 sends operation condition command.
 * - ACMD41 reads OCR
 *
 * \param v2   Shall be 1 if it is a SD card V2
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mci_op_cond(uint8_t v2)
{
    uint32_t arg, retry, resp;

    /*
     * Timeout 1s = 400KHz / ((6+6+6+6)*8) cylces = 2100 retry
     * 6 = cmd byte size
     * 6 = response byte size
     * 6 = cmd byte size
     * 6 = response byte size
     */
    retry = 2100;

    do {
        // CMD55 - Indicate to the card that the next command is an
        // application specific command rather than a standard command.
        if (!sd_mmc_spi_send_cmd(SDMMC_CMD55_APP_CMD, 0)) {
            return false;
        }

        // (ACMD41) Sends host OCR register
        arg = SD_MMC_VOLTAGE_SUPPORT;

        if (v2) {
            arg |= SD_ACMD41_HCS;
        }

        // Check response
        if (!sd_mmc_spi_send_cmd(SD_MCI_ACMD41_SD_SEND_OP_COND, arg)) {
            return false;
        }

        resp = sd_mmc_spi_get_response();

        if (resp & OCR_POWER_UP_BUSY) {
            // Card is ready
            if ((resp & OCR_CCS) != 0) {
                sd_mmc_card->type |= CARD_TYPE_HC;
            }

            break;
        }

        if (retry-- == 0) {
            return false;
        }
    } while (1);

    return true;
}
_STATIC bool sdio_op_cond(void)
{
    return true;          // No error but card type not updated
}
_STATIC bool sdio_get_max_speed(void)
{
    return false;
}
_STATIC bool sdio_cmd52_set_bus_width(void)
{
    return false;
}
_STATIC bool sdio_cmd52_set_high_speed(void)
{
    return false;
}

/**
 * \brief CMD6 for SD - Switch card in high speed mode
 *
 * \note CMD6 for SD is valid under the "trans" state.
 * \note sd_mmc_card->high_speed is updated.
 * \note sd_mmc_card->clock is updated.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_cm6_set_high_speed(void)
{
    uint8_t switch_status[SD_SW_STATUS_BSIZE];

    if (!sd_mmc_spi_adtc_start(SD_CMD6_SWITCH_FUNC,
                               SD_CMD6_MODE_SWITCH | SD_CMD6_GRP6_NO_INFLUENCE
                               | SD_CMD6_GRP5_NO_INFLUENCE | SD_CMD6_GRP4_NO_INFLUENCE
                               | SD_CMD6_GRP3_NO_INFLUENCE | SD_CMD6_GRP2_DEFAULT
                               | SD_CMD6_GRP1_HIGH_SPEED, SD_SW_STATUS_BSIZE, 1, true)) {
        return false;
    }

    if (!sd_mmc_spi_start_read_blocks(switch_status, 1)) {
        return false;
    }

    if (!sd_mmc_spi_wait_end_of_read_blocks()) {
        return false;
    }

    if (sd_mmc_spi_get_response() & CARD_STATUS_SWITCH_ERROR) {
        return false;
    }

    if (SD_SW_STATUS_FUN_GRP1_RC(switch_status)
        == SD_SW_STATUS_FUN_GRP_RC_ERROR) {
        // No supported, it is not a protocol error
        return true;
    }

    if (SD_SW_STATUS_FUN_GRP1_BUSY(switch_status)) {
        return false;
    }

    // CMD6 function switching period is within 8 clocks
    // after the end bit of status data.
    sd_mmc_spi_send_clock();
    sd_mmc_card->high_speed = 1;
    sd_mmc_card->clock *= 2;
    return true;
}

/**
 * \brief CMD6 for MMC - Switches the bus width mode
 *
 * \note CMD6 is valid under the "trans" state.
 * \note sd_mmc_card->bus_width is updated.
 *
 * \param bus_width   Bus width to set
 *
 * \return true if success, otherwise false
 */
_STATIC bool mmc_cmd6_set_bus_width(uint8_t bus_width)
{
    uint32_t arg;

    switch (bus_width) {
        case 8:
            arg = MMC_CMD6_ACCESS_SET_BITS | MMC_CMD6_INDEX_BUS_WIDTH
                  | MMC_CMD6_VALUE_BUS_WIDTH_8BIT;
            break;

        case 4:
            arg = MMC_CMD6_ACCESS_SET_BITS | MMC_CMD6_INDEX_BUS_WIDTH
                  | MMC_CMD6_VALUE_BUS_WIDTH_4BIT;
            break;

        default:
            arg = MMC_CMD6_ACCESS_SET_BITS | MMC_CMD6_INDEX_BUS_WIDTH
                  | MMC_CMD6_VALUE_BUS_WIDTH_1BIT;
            break;
    }

    if (!sd_mmc_spi_send_cmd(MMC_CMD6_SWITCH, arg)) {
        return false;
    }

    if (sd_mmc_spi_get_response() & CARD_STATUS_SWITCH_ERROR) {
        // No supported, it is not a protocol error
        return false;
    }

    sd_mmc_card->bus_width = bus_width;
    return true;
}

/**
 * \brief CMD6 for MMC - Switches in high speed mode
 *
 * \note CMD6 is valid under the "trans" state.
 * \note sd_mmc_card->high_speed is updated.
 * \note sd_mmc_card->clock is updated.
 *
 * \return true if success, otherwise false
 */
_STATIC bool mmc_cmd6_set_high_speed(void)
{
    if (!sd_mmc_spi_send_cmd(MMC_CMD6_SWITCH,
                             MMC_CMD6_ACCESS_WRITE_BYTE | MMC_CMD6_INDEX_HS_TIMING
                             | MMC_CMD6_VALUE_HS_TIMING_ENABLE)) {
        return false;
    }

    if (sd_mmc_spi_get_response() & CARD_STATUS_SWITCH_ERROR) {
        // No supported, it is not a protocol error
        return false;
    }

    sd_mmc_card->high_speed = 1;
    sd_mmc_card->clock = 52000000lu;
    return true;
}

/**
 * \brief CMD8 for SD card - Send Interface Condition Command.
 *
 * \note
 * Send SD Memory Card interface condition, which includes host supply
 * voltage information and asks the card whether card supports voltage.
 * Should be performed at initialization time to detect the card type.
 *
 * \param v2 Pointer to v2 flag to update
 *
 * \return true if success, otherwise false
 *         with a update of \ref sd_mmc_err.
 */
_STATIC bool sd_cmd8(uint8_t* v2)
{
    uint32_t resp;

    *v2 = 0;

    // Test for SD version 2
    if (!sd_mmc_spi_send_cmd(SD_CMD8_SEND_IF_COND,
                             SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
        return true;          // It is not a V2
    }

    // Check R7 response
    resp = sd_mmc_spi_get_response();

    if (resp == 0xFFFFFFFF) {
        // No compliance R7 value
        return true;          // It is not a V2
    }

    if ((resp & (SD_CMD8_MASK_PATTERN | SD_CMD8_MASK_VOLTAGE))
        != (SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
        return false;
    }

    *v2 = 1;
    return true;
}

/**
 * \brief CMD8 - The card sends its EXT_CSD register as a block of data.
 *
 * \param b_authorize_high_speed Pointer to update with the high speed
 * support information
 *
 * \return true if success, otherwise false
 */
_STATIC bool mmc_cmd8(uint8_t* b_authorize_high_speed)
{
    uint16_t i;
    uint32_t ext_csd;
    uint32_t sec_count;

    if (!sd_mmc_spi_adtc_start(MMC_CMD8_SEND_EXT_CSD, 0, EXT_CSD_BSIZE, 1,
                               false)) {
        return false;
    }

    //** Read and decode Extended Extended CSD
    // Note: The read access is done in byte to avoid a buffer
    // of EXT_CSD_BSIZE Byte in stack.

    // Read card type
    for (i = 0; i < (EXT_CSD_CARD_TYPE_INDEX + 4) / 4; i++) {
        if (!sd_mmc_spi_read_word(&ext_csd)) {
            return false;
        }
    }

    *b_authorize_high_speed = (ext_csd >> ((EXT_CSD_CARD_TYPE_INDEX % 4) * 8))
                              & MMC_CTYPE_52MHZ;

    if (MMC_CSD_C_SIZE(sd_mmc_card->csd) == 0xFFF) {
        // For high capacity SD/MMC card,
        // memory capacity = SEC_COUNT * 512 byte
        for (; i < (EXT_CSD_SEC_COUNT_INDEX + 4) / 4; i++) {
            if (!sd_mmc_spi_read_word(&sec_count)) {
                return false;
            }
        }

        sd_mmc_card->capacity = sec_count / 2;
    }

    for (; i < EXT_CSD_BSIZE / 4; i++) {
        if (!sd_mmc_spi_read_word(&sec_count)) {
            return false;
        }
    }

    return true;
}

/**
 * \brief CMD9: Addressed card sends its card-specific
 * data (CSD) on the CMD line spi.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_cmd9_spi(void)
{
    if (!sd_mmc_spi_adtc_start(SDMMC_SPI_CMD9_SEND_CSD,
                               (uint32_t) sd_mmc_card->rca << 16, CSD_REG_BSIZE, 1, true)) {
        return false;
    }

    if (!sd_mmc_spi_start_read_blocks(sd_mmc_card->csd, 1)) {
        return false;
    }

    return sd_mmc_spi_wait_end_of_read_blocks();
}

/**
 * \brief CMD9: Addressed card sends its card-specific
 * data (CSD) on the CMD line mci.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_cmd9_mci(void)
{
    if (!sd_mmc_spi_send_cmd(SDMMC_MCI_CMD9_SEND_CSD,
                             (uint32_t) sd_mmc_card->rca << 16)) {
        return false;
    }

    sd_mmc_spi_get_response_128(sd_mmc_card->csd);
    return true;
}

/**
 * \brief Decodes MMC CSD register
 */
_STATIC void mmc_decode_csd(void)
{
    uint32_t unit;
    uint32_t mul;
    uint32_t tran_speed;

    // Get MMC System Specification version supported by the card
    switch (MMC_CSD_SPEC_VERS(sd_mmc_card->csd)) {
        default:
        case 0:
            sd_mmc_card->version = CARD_VER_MMC_1_2;
            break;

        case 1:
            sd_mmc_card->version = CARD_VER_MMC_1_4;
            break;

        case 2:
            sd_mmc_card->version = CARD_VER_MMC_2_2;
            break;

        case 3:
            sd_mmc_card->version = CARD_VER_MMC_3;
            break;

        case 4:
            sd_mmc_card->version = CARD_VER_MMC_4;
            break;
    }

    // Get MMC memory max transfer speed in Hz.
    tran_speed = CSD_TRAN_SPEED(sd_mmc_card->csd);
    unit = sd_mmc_trans_units[tran_speed & 0x7];
    mul = mmc_trans_multipliers[(tran_speed >> 3) & 0xF];
    sd_mmc_card->clock = unit * mul * 1000;

    /*
     * Get card capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * memory capacity = BLOCKNR * BLOCK_LEN
     * Where
     * BLOCKNR = (C_SIZE+1) * MULT
     * MULT = 2 ^ (C_SIZE_MULT+2)       (C_SIZE_MULT < 8)
     * BLOCK_LEN = 2 ^ READ_BL_LEN      (READ_BL_LEN < 12)
     * ----------------------------------------------------
     * For high capacity SD/MMC card:
     * memory capacity = SEC_COUNT * 512 byte
     */
    if (MMC_CSD_C_SIZE(sd_mmc_card->csd) != 0xFFF) {
        uint32_t blocknr = ((MMC_CSD_C_SIZE(sd_mmc_card->csd) + 1)
                            * (1 << (MMC_CSD_C_SIZE_MULT(sd_mmc_card->csd) + 2)));
        sd_mmc_card->capacity = blocknr
                                * (1 << MMC_CSD_READ_BL_LEN(sd_mmc_card->csd)) / 1024;
    }
}

/**
 * \brief Decodes SD CSD register
 */
_STATIC void sd_decode_csd(void)
{
    uint32_t unit;
    uint32_t mul;
    uint32_t tran_speed;

    // Get SD memory maximum transfer speed in Hz.
    tran_speed = CSD_TRAN_SPEED(sd_mmc_card->csd);
    unit = sd_mmc_trans_units[tran_speed & 0x7];
    mul = sd_trans_multipliers[(tran_speed >> 3) & 0xF];
    sd_mmc_card->clock = unit * mul * 1000;

    /*
     * Get card capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * memory capacity = BLOCKNR * BLOCK_LEN
     * Where
     * BLOCKNR = (C_SIZE+1) * MULT
     * MULT = 2 ^ (C_SIZE_MULT+2)       (C_SIZE_MULT < 8)
     * BLOCK_LEN = 2 ^ READ_BL_LEN      (READ_BL_LEN < 12)
     * ----------------------------------------------------
     * For high capacity SD card:
     * memory capacity = (C_SIZE+1) * 512K byte
     */
    if (CSD_STRUCTURE_VERSION(sd_mmc_card->csd) >= SD_CSD_VER_2_0) {
        sd_mmc_card->capacity = (SD_CSD_2_0_C_SIZE(sd_mmc_card->csd) + 1) * 512;
    } else {
        uint32_t blocknr = ((SD_CSD_1_0_C_SIZE(sd_mmc_card->csd) + 1)
                            * (1 << (SD_CSD_1_0_C_SIZE_MULT(sd_mmc_card->csd) + 2)));
        sd_mmc_card->capacity = blocknr
                                * (1 << SD_CSD_1_0_READ_BL_LEN(sd_mmc_card->csd)) / 1024;
    }
}

/**
 * \brief CMD13 - Addressed card sends its status register.
 * This function waits the clear of the busy flag
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_cmd13(void)
{
    uint32_t nec_timeout;

    /* Wait for data ready status.
     * Nec timing: 0 to unlimited
     * However a timeout is used.
     * 200 000 * 8 cycles
     */
    nec_timeout = 200000;

    do {
        if (sd_mmc_is_spi()) {
            if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD13_SEND_STATUS, 0)) {
                return false;
            }

            // Check busy flag
            if (!(sd_mmc_spi_get_response() & 0xFF)) {
                break;
            }
        } else {
            if (!sd_mmc_spi_send_cmd(SDMMC_MCI_CMD13_SEND_STATUS,
                                     (uint32_t) sd_mmc_card->rca << 16)) {
                return false;
            }

            // Check busy flag
            if (sd_mmc_spi_get_response() & CARD_STATUS_READY_FOR_DATA) {
                break;
            }
        }

        if (nec_timeout-- == 0) {
            return false;
        }
    } while (1);

    return true;
}

/**
 * \brief ACMD6 - Define the data bus width to 4 bits bus
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_acmd6(void)
{
    // CMD55 - Indicate to the card that the next command is an
    // application specific command rather than a standard command.
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD55_APP_CMD,
                             (uint32_t) sd_mmc_card->rca << 16)) {
        return false;
    }

    // 10b = 4 bits bus
    if (!sd_mmc_spi_send_cmd(SD_ACMD6_SET_BUS_WIDTH, 0x2)) {
        return false;
    }

    sd_mmc_card->bus_width = 4;
    return true;
}

/**
 * \brief ACMD51 - Read the SD Configuration Register.
 *
 * \note
 * SD Card Configuration Register (SCR) provides information on the SD Memory
 * Card's special features that were configured into the given card. The size
 * of SCR register is 64 bits.
 *
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_acmd51(void)
{
    uint8_t scr[SD_SCR_REG_BSIZE];

    // CMD55 - Indicate to the card that the next command is an
    // application specific command rather than a standard command.
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD55_APP_CMD,
                             (uint32_t) sd_mmc_card->rca << 16)) {
        return false;
    }

    if (!sd_mmc_spi_adtc_start(SD_ACMD51_SEND_SCR, 0, SD_SCR_REG_BSIZE, 1,
                               true)) {
        return false;
    }

    if (!sd_mmc_spi_start_read_blocks(scr, 1)) {
        return false;
    }

    if (!sd_mmc_spi_wait_end_of_read_blocks()) {
        return false;
    }

    // Get SD Memory Card - Spec. Version
    switch (SD_SCR_SD_SPEC(scr)) {
        case SD_SCR_SD_SPEC_1_0_01:
            sd_mmc_card->version = CARD_VER_SD_1_0;
            break;

        case SD_SCR_SD_SPEC_1_10:
            sd_mmc_card->version = CARD_VER_SD_1_10;
            break;

        case SD_SCR_SD_SPEC_2_00:
            if (SD_SCR_SD_SPEC3(scr) == SD_SCR_SD_SPEC_3_00) {
                sd_mmc_card->version = CARD_VER_SD_3_0;
            } else {
                sd_mmc_card->version = CARD_VER_SD_2_0;
            }

            break;

        default:
            sd_mmc_card->version = CARD_VER_SD_1_0;
            break;
    }

    return true;
}

/**
 * \brief Select a card slot and initialize the associated sd_mmc_spi
 *
 * \param slot  Card slot number
 *
 * \retval SD_MMC_ERR_SLOT     Wrong slot number
 * \retval SD_MMC_ERR_NO_CARD  No card present on slot
 * \retval SD_MMC_ERR_UNUSABLE Unusable card
 * \retval SD_MMC_INIT_ONGOING Card initialization requested
 * \retval SD_MMC_OK           Card present
 */
_STATIC sd_mmc_err_t sd_mmc_select_slot(uint8_t slot)
{
    if (slot >= SD_MMC_MEM_CNT) {
        return SD_MMC_ERR_SLOT;
    }

    Assert(sd_mmc_nb_block_remaining == 0);

    if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_NO_CARD) {
        // A card plug on going, but this is not initialized
        sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_DEBOUNCE;
        // Debounce + Power On Setup
        SD_MMC_START_TIMEOUT();
        return SD_MMC_ERR_NO_CARD;
    }

    if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_DEBOUNCE) {
        if (!SD_MMC_IS_TIMEOUT()) {
            // Debounce on going
            return SD_MMC_ERR_NO_CARD;
        }

        // Card is not initialized
        sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_INIT;
        // Set 1-bit bus width and low clock for initialization
        sd_mmc_cards[slot].clock = SDMMC_CLOCK_INIT;
        sd_mmc_cards[slot].bus_width = 1;
        sd_mmc_cards[slot].high_speed = 0;
    }

    if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_UNUSABLE) {
        return SD_MMC_ERR_UNUSABLE;
    }

    // Initialize interface
    sd_mmc_slot_sel = slot;
    sd_mmc_card = &sd_mmc_cards[slot];
    sd_mmc_configure_slot();
    return (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_INIT) ?
           SD_MMC_INIT_ONGOING : SD_MMC_OK;
}

/**
 * \brief Configures the sd_mmc_spi with the selected card configuration
 */
_STATIC void sd_mmc_configure_slot(void)
{
    sd_mmc_spi_select_device(sd_mmc_slot_sel, sd_mmc_card->clock,
                             sd_mmc_card->bus_width, sd_mmc_card->high_speed);
}

/**
 * \brief Deselect the current card slot
 */
_STATIC void sd_mmc_deselect_slot(void)
{
    if (sd_mmc_slot_sel < SD_MMC_MEM_CNT) {
        sd_mmc_spi_deselect_device(sd_mmc_slot_sel);
    }
}

/**
 * \brief Initialize the SD card in SPI mode.
 *
 * \note
 * This function runs the initialization procedure and the identification
 * process, then it sets the SD/MMC card in transfer state.
 * At last, it will automaticly enable maximum bus width and transfer speed.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_spi_card_init(void)
{
    uint8_t v2 = 0;
    // In first, try to install SD/SDIO card
    sd_mmc_card->type = CARD_TYPE_SD;
    sd_mmc_card->version = CARD_VER_UNKNOWN;
    sd_mmc_card->rca = 0;

    // Card need of 74 cycles clock minimum to start
    sd_mmc_spi_send_clock();

    // CMD0 - Reset all cards to idle state.
    if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD0_GO_IDLE_STATE, 0)) {
        return false;
    }

    if (!sd_cmd8(&v2)) {
        return false;
    }

    // Try to get the SDIO card's operating condition
    if (!sdio_op_cond()) {
        return false;
    }

    if (sd_mmc_card->type & CARD_TYPE_SD) {
        // Try to get the SD card's operating condition
        if (!sd_spi_op_cond(v2)) {
            // It is not a SD card
            sd_mmc_card->type = CARD_TYPE_MMC;
            return sd_mmc_spi_install_mmc();
        }

        /* The CRC on card is disabled by default.
         * However, to be sure, the CRC OFF command is send.
         * Unfortunately, specific SDIO card does not support it
         * (H&D wireless card - HDG104 WiFi SIP)
         * and the command is send only on SD card.
         */
        if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD59_CRC_ON_OFF, 0)) {
            return false;
        }
    }

    // SD MEMORY
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        // Get the Card-Specific Data
        if (!sd_mmc_cmd9_spi()) {
            return false;
        }

        sd_decode_csd();

        // Read the SCR to get card version
        if (!sd_acmd51()) {
            return false;
        }
    }

    // SD MEMORY not HC, Set default block size
    if ((sd_mmc_card->type & CARD_TYPE_SD)
        && (0 == (sd_mmc_card->type & CARD_TYPE_HC))) {
        if (!sd_mmc_spi_send_cmd(SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
            return false;
        }
    }

    // Check communication
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!sd_mmc_cmd13()) {
            return false;
        }
    }

    // Reinitialize the slot with the new speed
    sd_mmc_configure_slot();
    return true;
}

/**
 * \brief Initialize the SD card in MCI mode.
 *
 * \note
 * This function runs the initialization procedure and the identification
 * process, then it sets the SD/MMC card in transfer state.
 * At last, it will automaticly enable maximum bus width and transfer speed.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_mci_card_init(void)
{
    uint8_t v2 = 0;

    // In first, try to install SD/SDIO card
    sd_mmc_card->type = CARD_TYPE_SD;
    sd_mmc_card->version = CARD_VER_UNKNOWN;
    sd_mmc_card->rca = 0;

    // Card need of 74 cycles clock minimum to start
    sd_mmc_spi_send_clock();

    // CMD0 - Reset all cards to idle state.
    if (!sd_mmc_spi_send_cmd(SDMMC_MCI_CMD0_GO_IDLE_STATE, 0)) {
        return false;
    }

    if (!sd_cmd8(&v2)) {
        return false;
    }

    // Try to get the SDIO card's operating condition
    if (!sdio_op_cond()) {
        return false;
    }

    if (sd_mmc_card->type & CARD_TYPE_SD) {
        // Try to get the SD card's operating condition
        if (!sd_mci_op_cond(v2)) {
            // It is not a SD card
            sd_mmc_card->type = CARD_TYPE_MMC;
            return sd_mmc_mci_install_mmc();
        }
    }

    if (sd_mmc_card->type & CARD_TYPE_SD) {
        // SD MEMORY, Put the Card in Identify Mode
        // Note: The CID is not used in this stack
        if (!sd_mmc_spi_send_cmd(SDMMC_CMD2_ALL_SEND_CID, 0)) {
            return false;
        }
    }

    // Ask the card to publish a new relative address (RCA).
    if (!sd_mmc_spi_send_cmd(SD_CMD3_SEND_RELATIVE_ADDR, 0)) {
        return false;
    }

    sd_mmc_card->rca = (sd_mmc_spi_get_response() >> 16) & 0xFFFF;

    // SD MEMORY, Get the Card-Specific Data
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!sd_mmc_cmd9_mci()) {
            return false;
        }

        sd_decode_csd();
    }

    // Select the and put it into Transfer Mode
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD7_SELECT_CARD_CMD,
                             (uint32_t) sd_mmc_card->rca << 16)) {
        return false;
    }

    // SD MEMORY, Read the SCR to get card version
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!sd_acmd51()) {
            return false;
        }
    }

    if ((4 <= sd_mmc_spi_get_bus_width(sd_mmc_slot_sel))) {
        // TRY to enable 4-bit mode
        if (sd_mmc_card->type & CARD_TYPE_SD) {
            if (!sd_acmd6()) {
                return false;
            }
        }

        // Switch to selected bus mode
        sd_mmc_configure_slot();
    }

    if (sd_mmc_spi_is_high_speed_capable()) {
        // TRY to enable High-Speed Mode
        if (sd_mmc_card->type & CARD_TYPE_SD) {
            if (sd_mmc_card->version > CARD_VER_SD_1_0) {
                if (!sd_cm6_set_high_speed()) {
                    return false;
                }
            }
        }

        // Valid new configuration
        sd_mmc_configure_slot();
    }

    // SD MEMORY, Set default block size
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!sd_mmc_spi_send_cmd(SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
            return false;
        }
    }

    return true;
}

/**
 * \brief Initialize the MMC card in SPI mode.
 *
 * \note
 * This function runs the initialization procedure and the identification
 * process, then it sets the SD/MMC card in transfer state.
 * At last, it will automaticly enable maximum bus width and transfer speed.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_spi_install_mmc(void)
{
    uint8_t b_authorize_high_speed;

    // CMD0 - Reset all cards to idle state.
    if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD0_GO_IDLE_STATE, 0)) {
        return false;
    }

    if (!mmc_spi_op_cond()) {
        return false;
    }

    // Disable CRC check for SPI mode
    if (!sd_mmc_spi_send_cmd(SDMMC_SPI_CMD59_CRC_ON_OFF, 0)) {
        return false;
    }

    // Get the Card-Specific Data
    if (!sd_mmc_cmd9_spi()) {
        return false;
    }

    mmc_decode_csd();

    // For MMC 4.0 Higher version
    if (sd_mmc_card->version >= CARD_VER_MMC_4) {
        // Get EXT_CSD
        if (!mmc_cmd8(&b_authorize_high_speed)) {
            return false;
        }
    }

    // Set default block size
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
        return false;
    }

    // Check communication
    if (!sd_mmc_cmd13()) {
        return false;
    }

    // Reinitialize the slot with the new speed
    sd_mmc_configure_slot();
    return true;
}

/**
 * \brief Initialize the MMC card in MCI mode.
 *
 * \note
 * This function runs the initialization procedure and the identification
 * process, then it sets the SD/MMC card in transfer state.
 * At last, it will automaticly enable maximum bus width and transfer speed.
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_mci_install_mmc(void)
{
    uint8_t b_authorize_high_speed;

    // CMD0 - Reset all cards to idle state.
    if (!sd_mmc_spi_send_cmd(SDMMC_MCI_CMD0_GO_IDLE_STATE, 0)) {
        return false;
    }

    if (!mmc_mci_op_cond()) {
        return false;
    }

    // Put the Card in Identify Mode
    // Note: The CID is not used in this stack
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD2_ALL_SEND_CID, 0)) {
        return false;
    }

    // Assign relative address to the card.
    sd_mmc_card->rca = 1;

    if (!sd_mmc_spi_send_cmd(MMC_CMD3_SET_RELATIVE_ADDR,
                             (uint32_t) sd_mmc_card->rca << 16)) {
        return false;
    }

    // Get the Card-Specific Data
    if (!sd_mmc_cmd9_mci()) {
        return false;
    }

    mmc_decode_csd();

    // Select the and put it into Transfer Mode
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD7_SELECT_CARD_CMD,
                             (uint32_t) sd_mmc_card->rca << 16)) {
        return false;
    }

    if (sd_mmc_card->version >= CARD_VER_MMC_4) {
        // For MMC 4.0 Higher version
        // Get EXT_CSD
        if (!mmc_cmd8(&b_authorize_high_speed)) {
            return false;
        }

        if (4 <= sd_mmc_spi_get_bus_width(sd_mmc_slot_sel)) {
            // Enable more bus width
            if (!mmc_cmd6_set_bus_width(
                    sd_mmc_spi_get_bus_width(sd_mmc_slot_sel))) {
                return false;
            }

            // Reinitialize the slot with the bus width
            sd_mmc_configure_slot();
        }

        if (sd_mmc_spi_is_high_speed_capable() && b_authorize_high_speed) {
            // Enable HS
            if (!mmc_cmd6_set_high_speed()) {
                return false;
            }

            // Reinitialize the slot with the new speed
            sd_mmc_configure_slot();
        }
    } else {
        // Reinitialize the slot with the new speed
        sd_mmc_configure_slot();
    }

    uint8_t retry = 10;

    while (retry--) {
        // Retry is a WORKAROUND for no compliance card (Atmel Internal ref. MMC19):
        // These cards seem not ready immediatly
        // after the end of busy of mmc_cmd6_set_high_speed()

        // Set default block size
        if (sd_mmc_spi_send_cmd(SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------
//--------------------- PUBLIC FUNCTIONS ----------------------------

void sd_mmc_init(void)
{
    for (uint8_t slot = 0; slot < SD_MMC_MEM_CNT; slot++) {
        sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_NO_CARD;
    }

    sd_mmc_slot_sel = 0xFF;          // No slot configurated
    sd_mmc_spi_init();
}

/** \brief Wait until SD is present (blocking function)
 *
 */
Ctrl_status sd_mmc_wait4SD()
{
    Ctrl_status status;

    // Wait card present and ready
    do {
        status = sd_mmc_test_unit_ready(0);

        if (CTRL_FAIL == status) {
            while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
            }
        }
    } while (CTRL_GOOD != status);

    return status;
}

/** \brief Mount disk, create a file and write header
 *
 *
 */
void buildSD(FIL* file_object, FATFS* fs)
{
    FRESULT res;
    char test_file_name[] = "0:sd_mmc_test.txt";
    memset(fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, fs);

    if (FR_INVALID_DRIVE == res) {
        while (1)
            ;
    }

    test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
    res = f_open(file_object, (char const*) test_file_name,
                 FA_CREATE_ALWAYS | FA_WRITE);

    if (res != FR_OK) {
        usart_write("FAIL\r\n");

        while (1)
            ;
    }

    if (0 == f_puts("Test SD/MMC stack\n", file_object)) {
        while (1)
            ;
    }
}

uint8_t sd_mmc_nb_slot(void)
{
    return SD_MMC_MEM_CNT;
}

sd_mmc_err_t sd_mmc_check(uint8_t slot)
{
    sd_mmc_err_t sd_mmc_err;
    sd_mmc_err = sd_mmc_select_slot(slot);

    if (sd_mmc_err != SD_MMC_INIT_ONGOING) {
        sd_mmc_deselect_slot();
        return sd_mmc_err;
    }

    // Initialization of the card requested
    if (sd_mmc_spi_card_init()) {
        sd_mmc_card->state = SD_MMC_CARD_STATE_READY;
        sd_mmc_deselect_slot();
        // To notify that the card has been just initialized
        // It is necessary for USB Device MSC
        return SD_MMC_INIT_ONGOING;
    }

    sd_mmc_card->state = SD_MMC_CARD_STATE_UNUSABLE;
    sd_mmc_deselect_slot();
    return SD_MMC_ERR_UNUSABLE;
}

card_type_t sd_mmc_get_type(uint8_t slot)
{
    if (SD_MMC_OK != sd_mmc_select_slot(slot)) {
        return CARD_TYPE_UNKNOWN;
    }

    sd_mmc_deselect_slot();
    return sd_mmc_card->type;
}

card_version_t sd_mmc_get_version(uint8_t slot)
{
    if (SD_MMC_OK != sd_mmc_select_slot(slot)) {
        return CARD_VER_UNKNOWN;
    }

    sd_mmc_deselect_slot();
    return sd_mmc_card->version;
}

uint32_t sd_mmc_get_capacity(uint8_t slot)
{
    if (SD_MMC_OK != sd_mmc_select_slot(slot)) {
        return 0;
    }

    sd_mmc_deselect_slot();
    return sd_mmc_card->capacity;
}

bool sd_mmc_is_write_protected(uint8_t slot)
{
    UNUSED(slot);
    return false;
}

sd_mmc_err_t sd_mmc_init_read_blocks(uint8_t slot, uint32_t start,
                                     uint16_t nb_block)
{
    sd_mmc_err_t sd_mmc_err;
    uint32_t cmd, arg, resp;

    sd_mmc_err = sd_mmc_select_slot(slot);

    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    // Wait for data ready status
    if (!sd_mmc_cmd13()) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    if (nb_block > 1) {
        cmd = SDMMC_CMD18_READ_MULTIPLE_BLOCK;
    } else {
        cmd = SDMMC_CMD17_READ_SINGLE_BLOCK;
    }

    /*
     * SDSC Card (CCS=0) uses byte unit address,
     * SDHC and SDXC Cards (CCS=1) use block unit address (512 Bytes unit).
     */
    if (sd_mmc_card->type & CARD_TYPE_HC) {
        arg = start;
    } else {
        arg = (start * SD_MMC_BLOCK_SIZE);
    }

    if (!sd_mmc_spi_adtc_start(cmd, arg, SD_MMC_BLOCK_SIZE, nb_block, true)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    // Check response
    if (sd_mmc_is_mci()) {
        resp = sd_mmc_spi_get_response();

        if (resp & CARD_STATUS_ERR_RD_WR) {
            sd_mmc_deselect_slot();
            return SD_MMC_ERR_COMM;
        }
    }

    sd_mmc_nb_block_remaining = nb_block;
    sd_mmc_nb_block_to_tranfer = nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_start_read_blocks(void* dest, uint16_t nb_block)
{
    Assert(sd_mmc_nb_block_remaining >= nb_block);

    if (!sd_mmc_spi_start_read_blocks(dest, nb_block)) {
        sd_mmc_nb_block_remaining = 0;
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_nb_block_remaining -= nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_wait_end_of_read_blocks(void)
{
    if (!sd_mmc_spi_wait_end_of_read_blocks()) {
        return SD_MMC_ERR_COMM;
    }

    if (sd_mmc_nb_block_remaining) {
        return SD_MMC_OK;
    }

    // All blocks are transfered then stop read operation
    if (sd_mmc_nb_block_to_tranfer == 1) {
        // Single block transfer, then nothing to do
        sd_mmc_deselect_slot();
        return SD_MMC_OK;
    }

    // WORKAROUND for no compliance card (Atmel Internal ref. !MMC7 !SD19):
    // The errors on this command must be ignored
    // and one retry can be necessary in SPI mode for no compliance card.
    if (!sd_mmc_spi_send_cmd(SDMMC_CMD12_STOP_TRANSMISSION, 0)) {
        sd_mmc_spi_send_cmd(SDMMC_CMD12_STOP_TRANSMISSION, 0);
    }

    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_init_write_blocks(uint8_t slot, uint32_t start,
                                      uint16_t nb_block)
{
    sd_mmc_err_t sd_mmc_err;
    uint32_t cmd, arg, resp;

    sd_mmc_err = sd_mmc_select_slot(slot);

    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    if (sd_mmc_is_write_protected(slot)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_WP;
    }

    if (nb_block > 1) {
        cmd = SDMMC_CMD25_WRITE_MULTIPLE_BLOCK;
    } else {
        cmd = SDMMC_CMD24_WRITE_BLOCK;
    }

    /*
     * SDSC Card (CCS=0) uses byte unit address,
     * SDHC and SDXC Cards (CCS=1) use block unit address (512 Bytes unit).
     */
    if (sd_mmc_card->type & CARD_TYPE_HC) {
        arg = start;
    } else {
        arg = (start * SD_MMC_BLOCK_SIZE);
    }

    if (!sd_mmc_spi_adtc_start(cmd, arg, SD_MMC_BLOCK_SIZE, nb_block, true)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    // Check response
    if (sd_mmc_is_mci()) {
        resp = sd_mmc_spi_get_response();

        if (resp & CARD_STATUS_ERR_RD_WR) {
            sd_mmc_deselect_slot();
            return SD_MMC_ERR_COMM;
        }
    }

    sd_mmc_nb_block_remaining = nb_block;
    sd_mmc_nb_block_to_tranfer = nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_start_write_blocks(const void* src, uint16_t nb_block)
{
    Assert(sd_mmc_nb_block_remaining >= nb_block);

    if (!sd_mmc_spi_start_write_blocks(src, nb_block)) {
        sd_mmc_nb_block_remaining = 0;
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_nb_block_remaining -= nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_wait_end_of_write_blocks(void)
{
    if (!sd_mmc_spi_wait_end_of_write_blocks()) {
        return SD_MMC_ERR_COMM;
    }

    if (sd_mmc_nb_block_remaining) {
        return SD_MMC_OK;
    }

    // All blocks are transfered then stop write operation
    if (sd_mmc_nb_block_to_tranfer == 1) {
        // Single block transfer, then nothing to do
        sd_mmc_deselect_slot();
        return SD_MMC_OK;
    }

    if (sd_mmc_is_mci()) {
        // Note: SPI multiblock writes terminate using a special
        // token, not a STOP_TRANSMISSION request.
        if (!sd_mmc_spi_adtc_stop(SDMMC_CMD12_STOP_TRANSMISSION, 0)) {
            sd_mmc_deselect_slot();
            return SD_MMC_ERR_COMM;
        }
    }

    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

//! @}
