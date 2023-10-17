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

void DhcpStateMachine (uint16_t len);
const uint32_t cookie = 0x630820363 ;
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

 uint32_t dhcpState = DHCP_STATE_INIT;
 uint8_t hostname[DHCP_HOSTNAME_MAX_LEN] = "Arduino-ENC28j60-00";   // Last two characters will be filled by last 2 MAC digits ;
 uint32_t currentXid;
 uint32_t stateTimer;
 uint32_t leaseStart;
 uint32_t leaseTime;
 uint32_t* bufPtr;

 uint32_t* dhcpCustomOptionList = NULL;

extern uint32_t allOnes[];// = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

 void addToBuf (uint32_t b) {
    *bufPtr++ = b;
}

 void addBytes (uint32_t len, const uint32_t* data) {
    while (len-- > 0)
        addToBuf(*data++);
}

 void addOption (uint32_t opt, uint32_t len, const uint32_t* data) {
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

 void send_dhcp_message(uint32_t *requestip) {

    memset(buffer, 0, UDP_DATA_P + sizeof( DHCPdata ));

    udpPrepare(DHCP_CLIENT_PORT,
                          (dhcpState == DHCP_STATE_BOUND ? dhcpip : allOnes),
                          DHCP_SERVER_PORT);

    buffer[42] = DHCP_BOOTREQUEST; // OPERATION
    buffer[43] = 1; //HTYPE
    buffer[44] = 6;// HLEN
    buffer[45] = 0;// HOPS
    buffer[46] =  0xDE; // XID
    buffer[47] =  0xAD; //XID
    buffer[48] = 0xBE;// XID
    buffer[49] = 0xEF;// XID
    buffer[50] =  0x0; // SECS
    buffer[51] =  0x0;// SECS
    buffer[52] = 0;// FLAGS
    buffer[53] = 0;// FLAGS
    buffer[54] =  myip[0]; // IP CLIENT
    buffer[55] =  myip[1];// IP CLIENT
    buffer[56] = myip[2];// IP CLIENT
    buffer[57] = myip[3];// IP CLIENT
    buffer[58] =  0x0; // MY IP
    buffer[59] =  0x0;// MY IP
    buffer[60] = 0x0;// MY IP
    buffer[61] = 0x0;// MY IP
    buffer[62] =  0x0; // IP SERVER
    buffer[63] =  0x0;// IP SERVER
    buffer[64] = 0x0;// IP SERVER
    buffer[65] = 0;// IP SERVER
    buffer[66] =  0x0; // IP GW
    buffer[67] =  0x0;// IP GW
    buffer[68] = 0;// IP GW
    buffer[69] = 0;// IP GW
    buffer[70] =  mymac[0]; // CH ADD [x16 15]
    buffer[71] =  mymac[1];// CH ADD [x16 14]
    buffer[72] = mymac[2];
    buffer[73] = mymac[3];
    buffer[74] = mymac[4];
    buffer[75] = mymac[5];
    memset(buffer + 76, 0, 10);   //10 bytes padding
    memset(buffer + 86, 0, 64);   //sname
    memset(buffer + 150, 0, 128); //sfile
    memset(buffer + 278, 0, 64);
    buffer[278] = 0x63; //cookie
    buffer[279] = 0x82;
    buffer[280] = 0x53;
    buffer[281] = 0x63;
    buffer[282] = 0x35; //0x35
    buffer[283] = 0x01; //len(1)
    buffer[284] = 0x01; //DHCP_DISOVERY
    buffer[285] = 0x3D; //CLIENT ID
    buffer[286] = 0x07; // LEN(7)
    buffer[287] = 0x01; // HTYPE
    buffer[288] = 0x1A;
    buffer[289] = 0x2B;
    buffer[290] = 0x3C;
    buffer[291] = 0x4D;// mac
    buffer[292] = 0x5E;
    buffer[293] = 0x6F; //mac
    buffer[294] = 0x0C; //HOSTNAME
    buffer[295] = 0x03; //len(3)
    buffer[296] = 0x41; //'A'
    buffer[297] = 0x41; //'A'
    buffer[298] = 0x41; //'A'
    buffer[299] = 0x37; // PARAM_REQ_LIST
    buffer[300] = 0x03; // len(3)
    buffer[301] = 0x01; // SUBNET
    buffer[302] = 0x03; //ROUTER
    buffer[303] = 0x06; // NAME_SERVER
    buffer[304] = 0xFF; // END OF FRAME
    // packet size will be under 300 uint32_ts
    udpTransmit(305);
}

 void process_dhcp_offer(uint16_t len, uint32_t *offeredip) {
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

 void process_dhcp_ack(uint16_t len) {
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

 bool dhcp_received_message_type (uint16_t len, uint32_t msgType) {
    if (len > 0){
    }
    #if 0
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
    #endif
    return false;
}

 char toAsciiHex(uint32_t b) {
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

    if (isLinkUp() == true){
        while (dhcpState != DHCP_STATE_BOUND){
            uint16_t packet_length = packetReceive();
            DhcpStateMachine(packet_length);
        };
    }
    #if 0
    updateBroadcastAddress();
    delaycnt = 0;
    return dhcpState == DHCP_STATE_BOUND ;
    #else
    return true;
    #endif
}

#if 0
void dhcpAddOptionCallback(uint32_t option, DhcpOptionCallback callback)
{
     uint32_t optionList[2];
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
        #if 0
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
        #endif
        dhcpState = DHCP_STATE_BOUND;
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
