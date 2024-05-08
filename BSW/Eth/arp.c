#include "stdint.h"
#include "stdbool.h"
#include "EtherCard.h"
#include "arp.h"

typedef struct ethernetdevice_s{
    uint8_t mac[6];
    uint8_t ip[4];
} ethernetdevice_st;

#define ARPTABLE_LEN 10u
ethernetdevice_st arptable[ARPTABLE_LEN];
uint8_t arptable_entries = 0;

void ARP_sendrequest(char * ip)
{
    uint8_t buffer[64];
    buffer[0] = 0xFF;
    buffer[1] = 0xFF;
    buffer[2] = 0xFF;
    buffer[3] = 0xFF;
    buffer[4] = 0xFF;
    buffer[5] = 0xFF;
    buffer[6] = 0x00;
    buffer[7] = 0xE9;
    buffer[8] = 0x3A;
    buffer[9] = 0x25;
    buffer[10] = 0xC2;
    buffer[11] = 0x29;
    buffer[12] = 0x08;
    buffer[13] = 0x06; // ARP TYPE
    /* *************** ARP FRAME **************** */
    buffer[14] = 0x00;
    buffer[15] = 0x01; //Ethernet
    buffer[16] = 0x08;
    buffer[17] = 0x00; //IPv4
    buffer[18] = 0x06; //Hardware size
    buffer[19] = 0x04; //Protocol size
    buffer[20] = 0x00;
    buffer[21] = 0x01; //Request
    buffer[22] = 0x00;
    buffer[23] = 0xE9;
    buffer[24] = 0x3A;
    buffer[25] = 0x25;
    buffer[26] = 0xC2;
    buffer[27] = 0x29;
    buffer[28] = 192;
    buffer[29] = 168;
    buffer[30] = 1;
    buffer[31] = 137;
    buffer[32] = 0x00;
    buffer[33] = 0x00;
    buffer[34] = 0x00;
    buffer[35] = 0x00;
    buffer[36] = 0x00;
    buffer[37] = 0x00;
    buffer[38] = ip[0];
    buffer[39] = ip[1];
    buffer[40] = ip[2];
    buffer[41] = ip[3];
    packetSend(42, buffer);
}

void ARP_sendreplyrouter(char * originip, char * originmac)
{
    uint8_t buffer[64];
    if( arptable_entries < ARPTABLE_LEN ){
        for(uint8_t i = 0; i < 6; i++){
            arptable[arptable_entries].mac[i] = originmac[i];
        }
        for(uint8_t i = 0; i < 4; i++){
            arptable[arptable_entries].ip[i] = originip[i];
        }
        arptable_entries++;
    }
    buffer[0] = originmac[0];
    buffer[1] = originmac[1];
    buffer[2] = originmac[2];
    buffer[3] = originmac[3];
    buffer[4] = originmac[4];
    buffer[5] = originmac[5];
    buffer[6] = 0x00;
    buffer[7] = 0xE9;
    buffer[8] = 0x3A;
    buffer[9] = 0x25;
    buffer[10] = 0xC2;
    buffer[11] = 0x29;
    buffer[12] = 0x08;
    buffer[13] = 0x06; // ARP TYPE
    /* *************** ARP FRAME **************** */
    buffer[14] = 0x00;
    buffer[15] = 0x01; //Ethernet
    buffer[16] = 0x08;
    buffer[17] = 0x00; //IPv4
    buffer[18] = 0x06; //Hardware size
    buffer[19] = 0x04; //Protocol size
    buffer[20] = 0x00;
    buffer[21] = 0x02; //Reply
    buffer[22] = 0x00;
    buffer[23] = 0xE9;
    buffer[24] = 0x3A;
    buffer[25] = 0x25;
    buffer[26] = 0xC2;
    buffer[27] = 0x29;
    buffer[28] = 192;
    buffer[29] = 168;
    buffer[30] = 1;
    buffer[31] = 137;
    buffer[32] = originmac[0];
    buffer[33] = originmac[1];
    buffer[34] = originmac[2];
    buffer[35] = originmac[3];
    buffer[36] = originmac[4];
    buffer[37] = originmac[5];
    buffer[38] = originip[0];
    buffer[39] = originip[1];
    buffer[40] = originip[2];
    buffer[41] = originip[3];
    packetSend(42, buffer);
}

bool arp_isResolved(uint8_t ip)
{
    bool result = false;
    for( uint8_t i = 0; i < arptable_entries; i++){
        if( arptable[i].ip[3] == ip ){
            result = true;
        }
    }
    return result;
}
