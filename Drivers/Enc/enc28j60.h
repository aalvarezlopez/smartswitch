/**
 * @file enc28j60.h
 * @brief This driver provides initialization and transmit/receive
 * functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-10-18
 */

 #ifndef ENC28J60_H
 #define ENC28J60_H

// buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated

 #define RXSTART_INIT        0x0000  /*! start of RX buffer, (must be zero, Rev. B4 Errata point 5) */
 #define RXSTOP_INIT         0x0BFF  /*! Rx buffer size set to 3071 (0xBFF). Max
                                         Eth frame is 1518 bytes */

 #define TXSTART_INIT        0x0C00  /*! TX buffer starts after RX buffer end */
 #define TXSTOP_INIT         0x11FF  /*! Remaining memory will be use as TX buffer */

#define ENC_MAX_BUF_SIZE 1500
#define MAX_RX_BUFFER_N 5

/** @brief ENC28J60 init task
 *
 * Initialize and configure the module to be able to start transmissions and
 * receive messages
 */
void ENC_Init(void);

/** @brief Periodic task to get any pending message and monitor the system
 */
void ENC_Task(void);

bool ENC_getnewentry(uint8_t * buffer);

/**   @brief  Initialise network interface
 *     @param  size Size of data buffer
 *     @param  macaddr Pointer to 6 byte hardware (MAC) address
 *     @param  csPin Arduino pin used for chip select (enable network interface SPI bus). Default = 8
 *     @return <i>uint32_t</i> ENC28J60 firmware version or zero on failure.
 */
void initialize (void);

/**   @brief  Check if network link is connected
 *     @return <i>bool</i> True if link is up
 */
 bool isLinkUp ();

/**   @brief  Sends data to network interface
 *     @param  len Size of data to send
 *     @note   Data buffer is shared by receive and transmit functions
 */
 void packetSend (uint16_t len, const uint8_t * buffer);

/**   @brief  Copy received packets to data buffer
 *     @return <i>uint16_t</i> Size of received data
 *     @note   Data buffer is shared by receive and transmit functions
 */
 uint16_t packetReceive (void);

/**   @brief  Put ENC28J60 in sleep mode
*/
 void powerDown();

/**   @brief  Wake ENC28J60 from sleep mode
*/
 void powerUp();

/**   @brief  Enable reception of broadcast messages
 *     @param  temporary Set true to temporarily enable broadcast
 *     @note   This will increase load on received data handling
 */
 void enableBroadcast(void);

/**   @brief  Disable reception of broadcast messages
 *     @param  temporary Set true to only disable if temporarily enabled
 *     @note   This will reduce load on received data handling
 */
 void disableBroadcast(void);

/**   @brief  Enables reception of multicast messages
 *     @note   This will increase load on received data handling
 */
 void enableMulticast ();
 #endif
