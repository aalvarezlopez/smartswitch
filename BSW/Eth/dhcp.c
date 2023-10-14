// DHCP lookup functions based on the udp client
// http://www.ietf.org/rfc/rfc2131.txt
//
// Author: Andrew Lindsay
// Rewritten and optimized by Jean-Claude Wippler, http://jeelabs.org/
//
// Rewritten dhcpStateMachine by Chris van den Hooven
// as to implement dhcp-renew when lease expires (jun 2012)
//
// Various modifications and bug fixes contributed by Victor Aprea (oct 2012)
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html
//
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

#include "EtherCard.h"
#include "net.h"

#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTRESPONSE 2

// DHCP Message Type (option 53) (ref RFC 2132)
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_DECLINE 4
#define DHCP_ACK 5
#define DHCP_NAK 6
#define DHCP_RELEASE 7

static const uint32_t cookie = 0x630820363 ;
// DHCP States for access in applications (ref RFC 2131)
enum {
    DHCP_STATE_INIT,
    DHCP_STATE_SELECTING,
    DHCP_STATE_REQUESTING,
    DHCP_STATE_BOUND,
    DHCP_STATE_RENEWING,
};

/*
   op            1  Message op code / message type.
                    1 = BOOTREQUEST, 2 = BOOTREPLY
   htype         1  Hardware address type, see ARP section in "Assigned
                    Numbers" RFC; e.g., '1' = 10mb ethernet.
   hlen          1  Hardware address length (e.g.  '6' for 10mb
                    ethernet).
   hops          1  Client sets to zero, optionally used by relay agents
                    when booting via a relay agent.
   xid           4  Transaction ID, a random number chosen by the
                    client, used by the client and server to associate
                    messages and responses between a client and a
                    server.
   secs          2  Filled in by client, seconds elapsed since client
                    began address acquisition or renewal process.
   flags         2  Flags (see figure 2).
   ciaddr        4  Client IP address; only filled in if client is in
                    BOUND, RENEW or REBINDING state and can respond
                    to ARP requests.
   yiaddr        4  'your' (client) IP address.
   siaddr        4  IP address of next server to use in bootstrap;
                    returned in DHCPOFFER, DHCPACK by server.
   giaddr        4  Relay agent IP address, used in booting via a
                    relay agent.
   chaddr       16  Client hardware address.
   sname        64  Optional server host name, null terminated string.
   file        128  Boot file name, null terminated string; "generic"
                    name or null in DHCPDISCOVER, fully qualified
                    directory-path name in DHCPOFFER.
   options     var  Optional parameters field.  See the options
                    documents for a list of defined options.
 */




// size 236
typedef struct {
    uint32_t op, htype, hlen, hops;
    uint32_t xid;
    uint16_t secs, flags;
    uint32_t ciaddr[IP_LEN], yiaddr[IP_LEN], siaddr[IP_LEN], giaddr[IP_LEN];
    uint32_t chaddr[16], sname[64], file[128];
    // options
} DHCPdata;

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

// timeouts im ms
#define DHCP_REQUEST_TIMEOUT 10000

#define DHCP_HOSTNAME_MAX_LEN 32

// RFC 2132 Section 3.3:
// The time value of 0xffffffff is reserved to represent "infinity".
#define DHCP_INFINITE_LEASE  0xffffffff

static uint32_t dhcpState = DHCP_STATE_INIT;
static uint8_t hostname[DHCP_HOSTNAME_MAX_LEN] = "Arduino-ENC28j60-00";   // Last two characters will be filled by last 2 MAC digits ;
static uint32_t currentXid;
static uint32_t stateTimer;
static uint32_t leaseStart;
static uint32_t leaseTime;
static uint32_t* bufPtr;

static uint32_t* dhcpCustomOptionList = NULL;

extern uint32_t allOnes[];// = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static void addToBuf (uint32_t b) {
    *bufPtr++ = b;
}

static void addBytes (uint32_t len, const uint32_t* data) {
    while (len-- > 0)
        addToBuf(*data++);
}

static void addOption (uint32_t opt, uint32_t len, const uint32_t* data) {
    addToBuf(opt);
    addToBuf(len);
    addBytes(len, data);
}


// Main DHCP sending function

// implemented
// state             / msgtype
// INIT              / DHCPDISCOVER
// SELECTING         / DHCPREQUEST
// BOUND (RENEWING)  / DHCPREQUEST

// ----------------------------------------------------------
// |              |SELECTING    |RENEWING     |INIT         |
// ----------------------------------------------------------
// |broad/unicast |broadcast    |unicast      |broadcast    |
// |server-ip     |MUST         |MUST NOT     |MUST NOT     | option 54
// |requested-ip  |MUST         |MUST NOT     |MUST NOT     | option 50
// |ciaddr        |zero         |IP address   |zero         |
// ----------------------------------------------------------

// options used (both send/receive)
#define DHCP_OPT_SUBNET_MASK            1
#define DHCP_OPT_ROUTERS                3
#define DHCP_OPT_DOMAIN_NAME_SERVERS    6
#define DHCP_OPT_HOSTNAME               12
#define DHCP_OPT_REQUESTED_ADDRESS      50
#define DHCP_OPT_LEASE_TIME             51
#define DHCP_OPT_MESSAGE_TYPE           53
#define DHCP_OPT_SERVER_IDENTIFIER      54
#define DHCP_OPT_PARAMETER_REQUEST_LIST 55
#define DHCP_OPT_RENEWAL_TIME           58
#define DHCP_OPT_CLIENT_IDENTIFIER      61
#define DHCP_OPT_END                    255

#define DHCP_HTYPE_ETHER 1

static void send_dhcp_message(uint32_t *requestip) {

    memset(buffer, 0, UDP_DATA_P + sizeof( DHCPdata ));

    udpPrepare(DHCP_CLIENT_PORT,
                          (dhcpState == DHCP_STATE_BOUND ? dhcpip : allOnes),
                          DHCP_SERVER_PORT);

    buffer[10] |= DHCP_BOOTREQUEST; // OPERATION
    buffer[10] <<= 8;
    buffer[10] |= 1; //HTYPE

    buffer[11] = 6;// HLEN
    buffer[11] <<= 8;
    buffer[11] |= 0;// HOPS
    buffer[11] <<= 8;
    buffer[11] |=  0xDE; // XID
    buffer[11] <<= 8;
    buffer[11] |=  0xAD; //XID

    buffer[12] = 0xBE;// XID
    buffer[12] <<= 8;
    buffer[12] |= 0xEF;// XID
    buffer[12] <<= 8;
    buffer[12] |=  0x0; // SECS
    buffer[12] <<= 8;
    buffer[12] |=  0x0;// SECS

    buffer[13] = 0;// FLAGS
    buffer[13] <<= 8;
    buffer[13] |= 0;// FLAGS
    buffer[13] <<= 8;
    buffer[13] |=  myip[0]; // IP CLIENT
    buffer[13] <<= 8;
    buffer[13] |=  myip[1];// IP CLIENT

    buffer[14] = myip[2];// IP CLIENT
    buffer[14] <<= 8;
    buffer[14] |= myip[3];// IP CLIENT
    buffer[14] <<= 8;
    buffer[14] |=  0x0; // MY IP
    buffer[14] <<= 8;
    buffer[14] |=  0x0;// MY IP

    buffer[15] = 0x0;// MY IP
    buffer[15] <<= 8;
    buffer[15] |= 0x0;// MY IP
    buffer[15] <<= 8;
    buffer[15] |=  0x0; // IP SERVER
    buffer[15] <<= 8;
    buffer[15] |=  0x0;// IP SERVER

    buffer[16] = 0x0;// IP SERVER
    buffer[16] <<= 8;
    buffer[16] |= 0;// IP SERVER
    buffer[16] <<= 8;
    buffer[16] |=  0x0; // IP GW
    buffer[16] <<= 8;
    buffer[16] |=  0x0;// IP GW

    buffer[17] = 0;// IP GW
    buffer[17] <<= 8;
    buffer[17] |= 0;// IP GW
    buffer[17] <<= 8;
    buffer[17] |=  mymac[0]; // CH ADD [x16 15]
    buffer[17] <<= 8;
    buffer[17] |=  mymac[1];// CH ADD [x16 14]

    buffer[18] = mymac[2];
    buffer[18] <<= 8;
    buffer[18] |= mymac[3];
    buffer[18] <<= 8;
    buffer[18] |= mymac[4];
    buffer[18] <<= 8;
    buffer[18] |= mymac[5];

    buffer[19] = 0; // 10 bytes padding
    buffer[20] = 0; // 10 bytes padding
    buffer[21] = 0; // 2 bytes padding |  bytes sname [x64]

    buffer[22] = 0;// sname [x64]
    buffer[23] = 0;// sname [x64]
    buffer[24] = 0;// sname [x64]
    buffer[25] = 0;// sname [x64]
    buffer[26] = 0;// sname [x64]
    buffer[27] = 0;// sname [x64]
    buffer[28] = 0;// sname [x64]
    buffer[29] = 0;// sname [x64]
    buffer[30] = 0;// sname [x64]
    buffer[31] = 0;// sname [x64]
    buffer[32] = 0;// sname [x64]
    buffer[33] = 0;// sname [x64]
    buffer[34] = 0;// sname [x64]
    buffer[35] = 0;// sname [x64]
    buffer[36] = 0;// sname [x64]
    buffer[37] = 0;// sname [x64] 2 bytes | sfile 2 bytes
    buffer[38] = 0;// sfile [x128]
    buffer[39] = 0;// sfile [x128]
    buffer[40] = 0;// sfile [x128]
    buffer[41] = 0;// sfile [x128]
    buffer[42] = 0;// sfile [x128]
    buffer[43] = 0;// sfile [x128]
    buffer[44] = 0;// sfile [x128]
    buffer[45] = 0;// sfile [x128]
    buffer[46] = 0;// sfile [x128]
    buffer[47] = 0;// sfile [x128]
    buffer[48] = 0;// sfile [x128]
    buffer[49] = 0;// sfile [x128]
    buffer[50] = 0;// sfile [x128]
    buffer[51] = 0;// sfile [x128]
    buffer[52] = 0;// sfile [x128]
    buffer[53] = 0;// sfile [x128]
    buffer[54] = 0;// sfile [x128]
    buffer[55] = 0;// sfile [x128]
    buffer[56] = 0;// sfile [x128]
    buffer[57] = 0;// sfile [x128]
    buffer[58] = 0;// sfile [x128]
    buffer[59] = 0;// sfile [x128]
    buffer[60] = 0;// sfile [x128]
    buffer[61] = 0;// sfile [x128]
    buffer[62] = 0;// sfile [x128]
    buffer[63] = 0;// sfile [x128]
    buffer[64] = 0;// sfile [x128]
    buffer[65] = 0;// sfile [x128]
    buffer[66] = 0;// sfile [x128]
    buffer[67] = 0;// sfile [x128]
    buffer[68] = 0;// sfile [x128]
    buffer[69] = 0x6382; //sfile [x128] 2 bytes cokie
    buffer[70] = 0x53633501; //cookie | 0x35 | len(1)
    buffer[71] = 0x013D0701; // DHCP_DISCORE | CLIENT_ID | len(7) | HTYPE_ETHER
    buffer[72] = 0x1A2B3C4D;// mac
    buffer[73] = 0x5E6F0C03 ;//mac | HOSTNAME | len(3)
    buffer[74] = 0x41414137; // HOSTNAME | PARAM_REQ_LIST
    buffer[75] = 0x03010306; // len(3) | SUBNET | ROUTERS | NAME_SERVER
    buffer[76] = 0xFF0000; // END OF FRAME | 0000
    // packet size will be under 300 uint32_ts
    udpTransmit(77*4);
}

static void process_dhcp_offer(uint16_t len, uint32_t *offeredip) {
    // Map struct onto payload
    DHCPdata *dhcpPtr = (DHCPdata*) (buffer + UDP_DATA_P);

    // Offered IP address is in yiaddr
    copyIp(offeredip, dhcpPtr->yiaddr);

    // Search for the server IP
    uint32_t *ptr = (uint32_t*) (dhcpPtr + 1) + 4;
    do {
        uint32_t option = *ptr++;
        uint32_t optionLen = *ptr++;
        if (option == DHCP_OPT_SERVER_IDENTIFIER) {
            copyIp(dhcpip, ptr);
            break;
        }
        ptr += optionLen;
    } while (ptr < buffer + len);
}

static void process_dhcp_ack(uint16_t len) {
    // Map struct onto payload
    DHCPdata *dhcpPtr = (DHCPdata*) (buffer + UDP_DATA_P);

    // Allocated IP address is in yiaddr
    copyIp(myip, dhcpPtr->yiaddr);

    // Scan through variable length option list identifying options we want
    uint32_t *ptr = (uint32_t*) (dhcpPtr + 1) + 4;
    bool done = false;
    do {
        uint32_t option = *ptr++;
        uint32_t optionLen = *ptr++;
        switch (option) {
        case DHCP_OPT_SUBNET_MASK:
            copyIp(netmask, ptr);
            break;
        case DHCP_OPT_ROUTERS:
            copyIp(gwip, ptr);
            break;
        case DHCP_OPT_DOMAIN_NAME_SERVERS:
            copyIp(dnsip, ptr);
            break;
        case DHCP_OPT_LEASE_TIME:
        case DHCP_OPT_RENEWAL_TIME:
            leaseTime = 0;
            for (uint32_t i = 0; i<4; i++)
                leaseTime = (leaseTime << 8) + ptr[i];
            if (leaseTime != DHCP_INFINITE_LEASE) {
                leaseTime *= 1000;      // milliseconds
            }
            break;
        case DHCP_OPT_END:
            done = true;
            break;
        default: {
            // Is is a custom configured option?
            if (dhcpCustomOptionList) {
                uint32_t *p = dhcpCustomOptionList;
                while (*p != 0) {
                    if (option == *p) {
                        break;
                    }
                    p++;
                }
            }
        }
    }
    ptr += optionLen;
}
while (!done && ptr < buffer + len);
}

static bool dhcp_received_message_type (uint16_t len, uint32_t msgType) {
    // Map struct onto payload
    DHCPdata *dhcpPtr = (DHCPdata*) (buffer + UDP_DATA_P);

    if (len >= 70 && buffer[UDP_SRC_PORT_L_P] == DHCP_SERVER_PORT &&
            dhcpPtr->xid == currentXid ) {

        uint32_t *ptr = (uint32_t*) (dhcpPtr + 1) + 4;
        do {
            uint32_t option = *ptr++;
            uint32_t optionLen = *ptr++;
            if(option == DHCP_OPT_MESSAGE_TYPE && *ptr == msgType ) {
                return true;
            }
            ptr += optionLen;
        } while (ptr < buffer + len);
    }
    return false;
}

static char toAsciiHex(uint32_t b) {
    char c = b & 0x0f;
    c += (c <= 9) ? '0' : 'A'-10;
    return c;
}

bool dhcpSetup (const char *hname, bool fromRam) {
    // Use during setup, as this discards all incoming requests until it returns.
    // That shouldn't be a problem, because we don't have an IPaddress yet.
    // Will try 60 secs to obtain DHCP-lease.

    using_dhcp = true;

    #if 0
    if(hname != NULL) {
        if(fromRam) {
            strncpy(hostname, hname, DHCP_HOSTNAME_MAX_LEN);
        }
        else {
            strncpy_P(hostname, hname, DHCP_HOSTNAME_MAX_LEN);
        }
    }
    else {
        #endif
    {
        // Set a unique hostname, use Arduino-?? with last octet of mac address
        hostname[strlen(hostname) - 2] = toAsciiHex(mymac[5] >> 4);   // Appends mac to last 2 digits of the hostname
        hostname[strlen(hostname) - 1] = toAsciiHex(mymac[5]);   // Even if it's smaller than the maximum <thus, strlen(hostname)>
    }

    dhcpState = DHCP_STATE_INIT;

    while (dhcpState != DHCP_STATE_BOUND){
        if (isLinkUp() == true){
            DhcpStateMachine(packetReceive());
        };
    }
    updateBroadcastAddress();
    delaycnt = 0;
    return dhcpState == DHCP_STATE_BOUND ;
}

#if 0
void dhcpAddOptionCallback(uint32_t option, DhcpOptionCallback callback)
{
    static uint32_t optionList[2];
    optionList[0] = option;
    optionList[1] = 0;
    dhcpCustomOptionList = optionList;
    dhcpCustomOptionCallback = callback;
}

void dhcpAddOptionCallback(uint32_t* optionlist, DhcpOptionCallback callback)
{
    dhcpCustomOptionList = optionlist;
    dhcpCustomOptionCallback = callback;
}

#endif
void DhcpStateMachine (uint16_t len)
{

    switch (dhcpState) {

    case DHCP_STATE_BOUND:
        //!@todo Due to millis() wrap-around, DHCP renewal may not work if leaseTime is larger than 49days
        if (0){
            send_dhcp_message(myip);
            dhcpState = DHCP_STATE_RENEWING;
        }
        break;

    case DHCP_STATE_INIT:
        memset(myip,0,IP_LEN*4); // force ip 0.0.0.0
        send_dhcp_message(NULL);
        enableBroadcast(true); //Temporarily enable broadcasts
        dhcpState = DHCP_STATE_SELECTING;
        break;

    case DHCP_STATE_SELECTING:
        if (dhcp_received_message_type(len, DHCP_OFFER)) {
            uint32_t offeredip[IP_LEN];
            process_dhcp_offer(len, offeredip);
            send_dhcp_message(offeredip);
            dhcpState = DHCP_STATE_REQUESTING;
        } else {
            if (0){
                dhcpState = DHCP_STATE_INIT;
            }
        }
        break;

    case DHCP_STATE_REQUESTING:
    case DHCP_STATE_RENEWING:
        if (dhcp_received_message_type(len, DHCP_ACK)) {
            disableBroadcast(true); //Disable broadcast after temporary enable
            process_dhcp_ack(len);
            if (gwip[0] != 0) setGwIp(gwip); // why is this? because it initiates an arp request
            dhcpState = DHCP_STATE_BOUND;
        } else {
            if (0){
                dhcpState = DHCP_STATE_INIT;
            }
        }
        break;

    }
}



