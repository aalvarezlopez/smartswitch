/**
 * \file
 *
 * \brief Common SPI interface for SD/MMC stack
 *
 * Copyright (c) 2012 - 2013 Atmel Corporation. All rights reserved.
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
#include "sd_mmc_spi.h"
#include "spi.h"
#include "debug.h"

#  define driver  spi
#  define spi_setup_device  spi_master_setup_device
#define SPI_MODE_0 0

//! Internal global error status
_STATIC sd_mmc_spi_errno_t sd_mmc_spi_err;

struct spi_device {
    //! Board specific select id
    unsigned int id;
};

_STATIC struct spi_device sd_mmc_spi_devices[1] = { { 1 } };

//! 32 bits response of the last command
_STATIC uint32_t sd_mmc_spi_response_32;
//! Current position (byte) of the transfer started by mci_adtc_start()
_STATIC uint32_t sd_mmc_spi_transfert_pos;
//! Size block requested by last mci_adtc_start()
_STATIC uint16_t sd_mmc_spi_block_size;
//! Total number of block requested by last mci_adtc_start()
_STATIC uint16_t sd_mmc_spi_nb_block;

_STATIC uint8_t sd_mmc_spi_crc7(uint8_t* buf, uint8_t size);
_STATIC bool sd_mmc_spi_wait_busy(void);
_STATIC bool sd_mmc_spi_start_read_block(void);
_STATIC void sd_mmc_spi_stop_read_block(void);
_STATIC void sd_mmc_spi_start_write_block(void);
_STATIC bool sd_mmc_spi_stop_write_block(void);
_STATIC bool sd_mmc_spi_stop_multiwrite_block(void);

#define DEFAULT_CHIP_ID 0
#define CONFIG_SPI_MASTER_DELAY_BCS 0

/**
 * \brief Calculates the CRC7
 *
 * \param buf     Buffer data to compute
 * \param size    Size of buffer data
 *
 * \return CRC7 computed
 */
_STATIC uint8_t sd_mmc_spi_crc7(uint8_t* buf, uint8_t size)
{
    uint8_t crc, value, i;

    crc = 0;

    while (size--) {
        value = *buf++;

        for (i = 0; i < 8; i++) {
            crc <<= 1;

            if ((value & 0x80) ^ (crc & 0x80)) {
                crc ^= 0x09;
            }

            value <<= 1;
        }
    }

    crc = (crc << 1) | 1;
    return crc;
}

/**
 * \brief Wait the end of busy on DAT0 line
 *
 * \return true if success, otherwise false
 */
_STATIC bool sd_mmc_spi_wait_busy(void)
{
    uint8_t line = 0xFF;

    /* Delay before check busy
     * Nbr timing minimum = 8 cylces
     */
    spi_read_packet(SPI, &line, 1);

    /* Wait end of busy signal
     * Nec timing: 0 to unlimited
     * However a timeout is used.
     * 200 000 * 8 cycles
     */
    uint32_t nec_timeout = 200000;
    spi_read_packet(SPI, &line, 1);

    do {
        spi_read_packet(SPI, &line, 1);

        if (!(nec_timeout--)) {
            return false;
        }
    } while (line != 0xFF);

    return true;
}

/**
 * \brief Sends the correct TOKEN on the line to start a read block transfer
 *
 * \return true if success, otherwise false
 *         with a update of \ref sd_mmc_spi_err.
 */
_STATIC bool sd_mmc_spi_start_read_block(void)
{
    uint32_t i;
    uint8_t token;

    Assert(!(sd_mmc_spi_transfert_pos % sd_mmc_spi_block_size));

    /* Wait for start data token:
     * The read timeout is the Nac timing.
     * Nac must be computed trough CSD values,
     * or it is 100ms for SDHC / SDXC
     * Compute the maximum timeout:
     * Frequency maximum = 25MHz
     * 1 byte = 8 cycles
     * 100ms = 312500 x spi_read_packet() maximum
     */
    token = 0;
    i = 500000;

    do {
        if (i-- == 0) {
            sd_mmc_spi_err = SD_MMC_SPI_ERR_READ_TIMEOUT;
            return false;
        }

        spi_read_packet(SPI, &token, 1);

        if (SPI_TOKEN_DATA_ERROR_VALID(token)) {
            Assert(SPI_TOKEN_DATA_ERROR_ERRORS & token);

            if (token
                & (SPI_TOKEN_DATA_ERROR_ERROR
                   | SPI_TOKEN_DATA_ERROR_ECC_ERROR
                   | SPI_TOKEN_DATA_ERROR_CC_ERROR)) {
                sd_mmc_spi_err = SD_MMC_SPI_ERR_READ_CRC;
            } else {
                sd_mmc_spi_err = SD_MMC_SPI_ERR_OUT_OF_RANGE;
            }

            return false;
        }
    } while (token != SPI_TOKEN_SINGLE_MULTI_READ);

    return true;
}

/**
 * \brief Executed the end of a read block transfer
 */
_STATIC void sd_mmc_spi_stop_read_block(void)
{
    uint8_t crc[2];
    // Read 16-bit CRC (not cheked)
    spi_read_packet(SPI, crc, 2);
}

/**
 * \brief Sends the correct TOKEN on the line to start a write block transfer
 */
_STATIC void sd_mmc_spi_start_write_block(void)
{
    uint8_t dummy = 0xFF;
    Assert(!(sd_mmc_spi_transfert_pos % sd_mmc_spi_block_size));
    // Delay before start write block:
    // Nwr timing minimum = 8 cylces
    spi_write_packet(SPI, &dummy, 1);
    // Send start token
    uint8_t token;

    if (1 == sd_mmc_spi_nb_block) {
        token = SPI_TOKEN_SINGLE_WRITE;
    } else {
        token = SPI_TOKEN_MULTI_WRITE;
    }

    spi_write_packet(SPI, &token, 1);
}

/**
 * \brief Waits the TOKEN which notify the end of write block transfer
 *
 * \return true if success, otherwise false
 *         with a update of \ref sd_mmc_spi_err.
 */
_STATIC bool sd_mmc_spi_stop_write_block(void)
{
    uint8_t resp;
    uint16_t crc;

    // Send CRC
    crc = 0xFFFF;          /// CRC is disabled in SPI mode
    spi_write_packet(SPI, (uint8_t*) &crc, 2);
    // Receiv data response token
    spi_read_packet(SPI, &resp, 1);

    if (!SPI_TOKEN_DATA_RESP_VALID(resp)) {
        sd_mmc_spi_err = SD_MMC_SPI_ERR;
        return false;
    }

    // Check data response
    switch (SPI_TOKEN_DATA_RESP_CODE(resp)) {
        case SPI_TOKEN_DATA_RESP_ACCEPTED:
            break;

        case SPI_TOKEN_DATA_RESP_CRC_ERR:
            sd_mmc_spi_err = SD_MMC_SPI_ERR_WRITE_CRC;
            return false;

        case SPI_TOKEN_DATA_RESP_WRITE_ERR:
        default:
            sd_mmc_spi_err = SD_MMC_SPI_ERR_WRITE;
            return false;
    }

    return true;
}

/**
 * \brief Executed the end of a multi blocks write transfer
 *
 * \return true if success, otherwise false
 *         with a update of \ref sd_mmc_spi_err.
 */
_STATIC bool sd_mmc_spi_stop_multiwrite_block(void)
{
    uint8_t value;

    if (1 == sd_mmc_spi_nb_block) {
        return true;          // Single block write
    }

    if (sd_mmc_spi_nb_block
        > (sd_mmc_spi_transfert_pos / sd_mmc_spi_block_size)) {
        return true;          // It is not the End of multi write
    }

    // Delay before start write block:
    // Nwr timing minimum = 8 cylces
    value = 0xFF;
    spi_write_packet(SPI, &value, 1);
    // Send stop token
    value = SPI_TOKEN_STOP_TRAN;
    spi_write_packet(SPI, &value, 1);

    // Wait busy
    if (!sd_mmc_spi_wait_busy()) {
        sd_mmc_spi_err = SD_MMC_SPI_ERR_WRITE_TIMEOUT;
        return false;
    }

    return true;
}

//-------------------------------------------------------------------
//--------------------- PUBLIC FUNCTIONS ----------------------------

sd_mmc_spi_errno_t sd_mmc_spi_get_errno(void)
{
    return sd_mmc_spi_err;
}

void sd_mmc_spi_init(void)
{
    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    // Initialize SPI interface and enable it
#if defined(SD_MMC_SPI_USES_USART_SPI_SERVICE)
    usart_spi_init(SPI);
#else

    if (!spi_is_enabled(SPI)) {
        spi_master_init(SPI);
        spi_enable(SPI);
    }

#endif
}

void sd_mmc_spi_select_device(uint8_t slot, uint32_t clock, uint8_t bus_width,
                              bool high_speed)
{
    UNUSED(bus_width);
    UNUSED(high_speed);
    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    spi_master_setup_device(SPI, &sd_mmc_spi_devices[slot],
                            SPI_MODE_0, clock, 0);
    spi_select_device(SPI, &sd_mmc_spi_devices[slot]);
}

void sd_mmc_spi_deselect_device(uint8_t slot)
{
    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    spi_deselect_device(SPI, &sd_mmc_spi_devices[slot]);
}

void sd_mmc_spi_send_clock(void)
{
    uint8_t i;
    uint8_t dummy = 0xFF;

    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;

    //! Send 80 cycles
    for (i = 0; i < 10; i++) {
        spi_write_packet(SPI, &dummy, 1);          // 8 cycles
    }
}

bool sd_mmc_spi_send_cmd(sdmmc_cmd_def_t cmd, uint32_t arg)
{
    return sd_mmc_spi_adtc_start(cmd, arg, 0, 0, false);
}

bool sd_mmc_spi_adtc_start(sdmmc_cmd_def_t cmd, uint32_t arg,
                           uint16_t block_size, uint16_t nb_block, bool access_block)
{
    uint8_t dummy = 0xFF;
    uint8_t cmd_token[6];
    uint8_t ncr_timeout;
    uint8_t r1;          //! R1 response
    UNUSED(access_block);
    Assert(cmd & SDMMC_RESP_PRESENT);          // Always a response in SPI mode
    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;

    // Encode SPI command
    cmd_token[0] = SPI_CMD_ENCODE(SDMMC_CMD_GET_INDEX(cmd));
    cmd_token[1] = arg >> 24;
    cmd_token[2] = arg >> 16;
    cmd_token[3] = arg >> 8;
    cmd_token[4] = arg;
    cmd_token[5] = sd_mmc_spi_crc7(cmd_token, 5);

    // 8 cycles to respect Ncs timing
    // Note: This byte does not include start bit "0",
    // thus it is ignored by card.
    spi_write_packet(SPI, &dummy, 1);
    // Send command
    spi_write_packet(SPI, cmd_token, sizeof(cmd_token));
    // Wait for response
    // Two retry will be done to manage the Ncr timing between command and reponse
    // Ncr: Min. 1x8 clock  cycle, Max. 8x8 clock cycles
    // WORKAROUND for no compliance card (Atmel Internal ref. SD13):
    r1 = 0xFF;
    // Ignore first byte because Ncr min. = 8 clock cylces
    spi_read_packet(SPI, &r1, 1);
    ncr_timeout = 7;

    while (1) {
        spi_read_packet(SPI, &r1, 1);          // 8 cycles

        if ((r1 & R1_SPI_ERROR) == 0) {
            // Valid R1 response
            break;
        }

        if (--ncr_timeout == 0) {
            while (1)
                ;

            // Here Valid R1 response received
            sd_mmc_spi_err = SD_MMC_SPI_ERR_RESP_TIMEOUT;
            return false;
        }
    }

    // Save R1 (Specific to SPI interface) in 32 bit response
    // The R1_SPI_IDLE bit can be checked by high level
    sd_mmc_spi_response_32 = r1;

    // Manage error in R1
    if (r1 & R1_SPI_COM_CRC) {
        sd_mmc_spi_err = SD_MMC_SPI_ERR_RESP_CRC;
        return false;
    }

    if (r1 & R1_SPI_ILLEGAL_COMMAND) {
        sd_mmc_spi_err = SD_MMC_SPI_ERR_ILLEGAL_COMMAND;
        return false;
    }

    if (r1 & ~R1_SPI_IDLE) {
        // Other error
        sd_mmc_spi_err = SD_MMC_SPI_ERR;
        return false;
    }

    // Manage other responses
    if (cmd & SDMMC_RESP_BUSY) {
        if (!sd_mmc_spi_wait_busy()) {
            sd_mmc_spi_err = SD_MMC_SPI_ERR_RESP_BUSY_TIMEOUT;
            return false;
        }
    }

    if (cmd & SDMMC_RESP_8) {
        sd_mmc_spi_response_32 = 0;
        spi_read_packet(SPI, (uint8_t*) &sd_mmc_spi_response_32, 1);
        sd_mmc_spi_response_32 = le32_to_cpu(sd_mmc_spi_response_32);
    }

    if (cmd & SDMMC_RESP_32) {
        spi_read_packet(SPI, (uint8_t*) &sd_mmc_spi_response_32, 4);
        sd_mmc_spi_response_32 = be32_to_cpu(sd_mmc_spi_response_32);
    }

    sd_mmc_spi_block_size = block_size;
    sd_mmc_spi_nb_block = nb_block;
    sd_mmc_spi_transfert_pos = 0;
    return true;          // Command complete
}

uint32_t sd_mmc_spi_get_response(void)
{
    return sd_mmc_spi_response_32;
}

bool sd_mmc_spi_read_word(uint32_t* value)
{
    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    Assert(
        sd_mmc_spi_nb_block
        > (sd_mmc_spi_transfert_pos / sd_mmc_spi_block_size));

    if (!(sd_mmc_spi_transfert_pos % sd_mmc_spi_block_size)) {
        // New block
        if (!sd_mmc_spi_start_read_block()) {
            return false;
        }
    }

    // Read data
    spi_read_packet(SPI, (uint8_t*) value, 4);
    *value = le32_to_cpu(*value);
    sd_mmc_spi_transfert_pos += 4;

    if (!(sd_mmc_spi_transfert_pos % sd_mmc_spi_block_size)) {
        // End of block
        sd_mmc_spi_stop_read_block();
    }

    return true;
}

bool sd_mmc_spi_write_word(uint32_t value)
{
    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    Assert(
        sd_mmc_spi_nb_block
        > (sd_mmc_spi_transfert_pos / sd_mmc_spi_block_size));

    if (!(sd_mmc_spi_transfert_pos % sd_mmc_spi_block_size)) {
        // New block
        sd_mmc_spi_start_write_block();
    }

    // Write data
    value = cpu_to_le32(value);
    spi_write_packet(SPI, (uint8_t*) &value, 4);
    sd_mmc_spi_transfert_pos += 4;

    if (!(sd_mmc_spi_transfert_pos % sd_mmc_spi_block_size)) {
        // End of block
        if (!sd_mmc_spi_stop_write_block()) {
            return false;
        }

        // Wait busy due to data programmation
        if (!sd_mmc_spi_wait_busy()) {
            sd_mmc_spi_err = SD_MMC_SPI_ERR_WRITE_TIMEOUT;
            return false;
        }
    }

    return sd_mmc_spi_stop_multiwrite_block();
}

bool sd_mmc_spi_start_read_blocks(void* dest, uint16_t nb_block)
{
    uint32_t pos;

    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    pos = 0;

    while (nb_block--) {
        Assert(
            sd_mmc_spi_nb_block
            > (sd_mmc_spi_transfert_pos / sd_mmc_spi_block_size));

        if (!sd_mmc_spi_start_read_block()) {
            return false;
        }

        // Read block
        spi_read_packet(SPI, &((uint8_t*) dest)[pos], sd_mmc_spi_block_size);
        pos += sd_mmc_spi_block_size;
        sd_mmc_spi_transfert_pos += sd_mmc_spi_block_size;

        sd_mmc_spi_stop_read_block();
    }

    return true;
}

bool sd_mmc_spi_wait_end_of_read_blocks(void)
{
    return true;
}

bool sd_mmc_spi_start_write_blocks(const void* src, uint16_t nb_block)
{
    uint32_t pos;

    sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;
    pos = 0;

    while (nb_block--) {
        Assert(
            sd_mmc_spi_nb_block
            > (sd_mmc_spi_transfert_pos / sd_mmc_spi_block_size));
        sd_mmc_spi_start_write_block();

        // Write block
        spi_write_packet(SPI, &((uint8_t*) src)[pos], sd_mmc_spi_block_size);
        pos += sd_mmc_spi_block_size;
        sd_mmc_spi_transfert_pos += sd_mmc_spi_block_size;

        if (!sd_mmc_spi_stop_write_block()) {
            return false;
        }

        // Do not check busy of last block
        // but delay it to mci_wait_end_of_write_blocks()
        if (nb_block) {
            // Wait busy due to data programmation
            if (!sd_mmc_spi_wait_busy()) {
                sd_mmc_spi_err = SD_MMC_SPI_ERR_WRITE_TIMEOUT;
                return false;
            }
        }
    }

    return true;
}

bool sd_mmc_spi_wait_end_of_write_blocks(void)
{
    // Wait busy due to data programmation of last block writed
    if (!sd_mmc_spi_wait_busy()) {
        sd_mmc_spi_err = SD_MMC_SPI_ERR_WRITE_TIMEOUT;
        return false;
    }

    return sd_mmc_spi_stop_multiwrite_block();
}

//! @}
