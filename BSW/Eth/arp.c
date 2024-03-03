#include "stdint.h"
#include "stdbool.h"
#include "EtherCard.h"
#include "arp.h"

void ARP_sendrequest(void)
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
    buffer[38] = 192;
    buffer[39] = 168;
    buffer[40] = 1;
    buffer[41] = 137;
    packetSend(42, buffer);
}

void ARP_sendreplyrouter(void)
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
    buffer[32] = 0xF4;
    buffer[33] = 0x69;
    buffer[34] = 0x42;
    buffer[35] = 0x09;
    buffer[36] = 0x14;
    buffer[37] = 0xD0;
    buffer[38] = 192;
    buffer[39] = 168;
    buffer[40] = 1;
    buffer[41] = 1;
    packetSend(42, buffer);
}
void ARP_sendreplydevice(void)
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
    buffer[21] = 0x02; //Request
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
    buffer[32] = 0x48;
    buffer[33] = 0xBA;
    buffer[34] = 0x4E;
    buffer[35] = 0xAF;
    buffer[36] = 0x87;
    buffer[37] = 0x5C;
    buffer[38] = 192;
    buffer[39] = 168;
    buffer[40] = 1;
    buffer[41] = 33;
    packetSend(42, buffer);
}
