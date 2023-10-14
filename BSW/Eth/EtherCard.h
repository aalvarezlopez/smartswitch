
 #ifndef EtherCard_h
 #define EtherCard_h

 #include "bufferfiller.h"
 #include "enc28j60.h"
 #include "net.h"

/** Enable client connections.
 * Setting this to zero means that the program cannot issue TCP client requests
 * anymore. Compilation will still work but the request will never be
 * issued. Saves 4 bytes SRAM and 550 byte flash.
 */
 #define ETHERCARD_TCPCLIENT 1

/** Enable TCP server functionality.
 *   Setting this to zero means that the program will not accept TCP client
 *   requests. Saves 2 bytes SRAM and 250 bytes flash.
 */
 #define ETHERCARD_TCPSERVER 1

/** Enable UDP server functionality.
 *   If zero UDP server is disabled. It is
 *   still possible to register callbacks but these will never be called. Saves
 *   about 40 bytes SRAM and 200 bytes flash. If building with -flto this does not
 *   seem to save anything; maybe the linker is then smart enough to optimize the
 *   call away.
 */
 #define ETHERCARD_UDPSERVER 1

/** Enable automatic reply to pings.
 *   Setting to zero means that the program will not automatically answer to
 *   PINGs anymore. Also the callback that can be registered to answer incoming
 *   pings will not be called. Saves 2 bytes SRAM and 230 bytes flash.
 */
 #define ETHERCARD_ICMP 1


extern uint16_t delaycnt;
extern uint32_t gwip[];   // gateway
extern uint32_t broadcastip[]; // broadcast address
extern uint32_t mymac[];  // my MAC address
extern uint32_t myip[];   // my ip address
extern uint32_t netmask[]; // subnet mask
extern bool using_dhcp;
extern uint32_t dhcpip[]; // dhcp server
extern uint32_t dnsip[];  // dns server


uint32_t Ethernet_begin (const uint16_t size, const uint32_t* macaddr);

/**   @brief  Configure network interface with static IP
 *     @param  my_ip IP address (4 bytes). 0 for no change.
 *     @param  gw_ip Gateway address (4 bytes). 0 for no change. Default = 0
 *     @param  dns_ip DNS address (4 bytes). 0 for no change. Default = 0
 *     @param  mask Subnet mask (4 bytes). 0 for no change. Default = 0
 *     @return <i>bool</i> Returns true on success - actually always true
 */
bool Ethernet_staticSetup (const uint32_t* my_ip,
    const uint32_t* gw_ip,
    const uint32_t* dns_ip,
    const uint32_t* mask);

// tcpip.cpp
/**   @brief  Sends a UDP packet to the IP address of last processed received packet
 *     @param  data Pointer to data payload
 *     @param  len Size of data payload (max 220)
 *     @param  port Source IP port
 */
void EthertemakeUdpReply (const char *data, uint32_t len, uint16_t port);
/**   @brief  Convert a 16-bit integer into a string
 *     @param  value The number to convert
 *     @param  ptr The string location to write to
 */
char* Ethernet_wtoa(uint16_t value, char* ptr);


 #endif
