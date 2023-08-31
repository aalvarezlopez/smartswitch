/**
 * \file
 *
 * \brief Common IOPORT service main header file for AVR, UC3 and ARM
 *        architectures.
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
#ifndef IOPORT_H
#define IOPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup ioport_group Common IOPORT API
 *
 * See \ref ioport_quickstart.
 *
 * This is common IOPORT service for GPIO pin configuration and control in a
 * standardized manner across the MEGA, MEGA_RF, XMEGA, UC3 and ARM devices.
 *
 * Port pin control code is optimized for each platform, and should produce
 * both compact and fast execution times when used with constant values.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for clock speed and functions.
 * @{
 */

/**
 * \def IOPORT_CREATE_PIN(port, pin)
 * \brief Create IOPORT pin number
 *
 * Create a IOPORT pin number for use with the IOPORT functions.
 *
 * \param port IOPORT port (e.g. PORTA, PA or PIOA depending on chosen
 *             architecture)
 * \param pin IOPORT zero-based index of the I/O pin
 */

/** \brief IOPORT pin directions */
enum ioport_direction {
	IOPORT_DIR_INPUT,  /*!< IOPORT input direction */
	IOPORT_DIR_OUTPUT, /*!< IOPORT output direction */
};

/** \brief IOPORT levels */
enum ioport_value {
	IOPORT_PIN_LEVEL_LOW,  /*!< IOPORT pin value low */
	IOPORT_PIN_LEVEL_HIGH, /*!< IOPORT pin value high */
};

/** \brief IOPORT edge sense modes */
enum ioport_sense {
	IOPORT_SENSE_BOTHEDGES, /*!< IOPORT sense both rising and falling edges */
	IOPORT_SENSE_FALLING,   /*!< IOPORT sense falling edges */
	IOPORT_SENSE_RISING,    /*!< IOPORT sense rising edges */
	IOPORT_SENSE_LEVEL_LOW, /*!< IOPORT sense low level  */
	IOPORT_SENSE_LEVEL_HIGH,/*!< IOPORT sense High level  */
};

#endif
