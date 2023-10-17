#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "EtherCard.h"

uint32_t mymac[ETH_LEN];  // my MAC address
uint32_t myip[IP_LEN];   // my ip address
uint32_t netmask[IP_LEN]; // subnet mask
uint32_t broadcastip[IP_LEN]; // broadcast address
uint32_t gwip[IP_LEN];   // gateway
uint32_t dhcpip[IP_LEN]; // dhcp server
uint32_t dnsip[IP_LEN];  // dns server
uint32_t hisip[IP_LEN];  // ip address of remote host
uint16_t hisport = HTTP_PORT; // tcp port to browse to
bool using_dhcp = false;
bool persist_tcp_connection = false;
uint16_t delaycnt = 0; //request gateway ARP lookup

uint32_t EtherCard_begin (const uint16_t size,
                          const uint32_t* macaddr)
{
    using_dhcp = false;
    copyMac(mymac, macaddr);
    myip[0] = 192;
    myip[1] = 168;
    myip[2] = 1;
    myip[3] = 139;
    return initialize(size, mymac);
}

bool EtherCard_staticSetup (const uint32_t* my_ip,
                             const uint32_t* gw_ip,
                             const uint32_t* dns_ip,
                             const uint32_t* mask) {
    using_dhcp = false;

    if (my_ip != 0)
        copyIp(myip, my_ip);
    if (gw_ip != 0)
        setGwIp(gw_ip);
    if (dns_ip != 0)
        copyIp(dnsip, dns_ip);
    if(mask != 0)
        copyIp(netmask, mask);
    updateBroadcastAddress();
    delaycnt = 0; //request gateway ARP lookup
    return true;
}

void copyIp (uint32_t *dst, const uint32_t *src) {
    memcpy(dst, src, IP_LEN);
}

void copyMac (uint32_t *dst, const uint32_t *src) {
    memcpy(dst, src, ETH_LEN * 4);
}

// search for a string of the form key=value in
// a string that looks like q?xyz=abc&uvw=defgh HTTP/1.1\r\n
//
// The returned value is stored in strbuf. You must allocate
// enough storage for strbuf, maxlen is the size of strbuf.
// I.e the value it is declared with: strbuf[5]-> maxlen=5
uint32_t findKeyVal (const char *str,char *strbuf, uint32_t maxlen,const char *key)
{
    uint32_t found = false;
    uint32_t i=0;
    const char *kp;
    kp=key;
    while(*str &&  *str!=' ' && *str!='\n' && !found) {
        if (*str == *kp) {
            kp++;
            if (*kp == '\0') {
                str++;
                kp=key;
                if (*str == '=') {
                    found = true;
                }
            }
        } else {
            kp=key;
        }
        str++;
    }
    if (found) {
        // copy the value to a buffer and terminate it with '\0'
        while(*str &&  *str!=' ' && *str!='\n' && *str!='&' && i<maxlen-1) {
            *strbuf=*str;
            i++;
            str++;
            strbuf++;
        }
        *strbuf='\0';
    }
    // return the length of the value
    return(i);
}

// convert a single character to a 2 digit hex str
// a terminating '\0' is added
void int2h(char c, char *hstr)
{
    hstr[1]=(c & 0xf)+'0';
    if ((c & 0xf) >9) {
        hstr[1]=(c & 0xf) - 10 + 'a';
    }
    c=(c>>4)&0xf;
    hstr[0]=c+'0';
    if (c > 9) {
        hstr[0]=c - 10 + 'a';
    }
    hstr[2]='\0';
}
