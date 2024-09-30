/**
 * @file enc28j60.c
 * @brief
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-10-06
 */

#include "stdint.h"
#include "stdbool.h"
#include "enc28j60_pri.h"
#include "enc28j60.h"

#include "spi.h"
#include "delays.h"

uint32_t Enc28j60Bank = 0;
volatile uint8_t _enc28_rev = 0;
volatile uint8_t g_enc28_erxfcon = 0;
uint8_t rxbuffer[MAX_RX_BUFFER_N][ENC_MAX_BUF_SIZE];
uint8_t rxbuffersize[MAX_RX_BUFFER_N];
uint8_t nextrxslot = 0;
uint8_t latestreadslot = 0;
uint8_t macaddr[6] = {0x00, 0xE9, 0x3A, 0x25, 0xC2, 0x29};

uint32_t readRegByte (uint32_t address);

void ENC_Init(void)
{
    for(uint8_t i = 0; i < MAX_RX_BUFFER_N; i++){
        rxbuffersize[i] = 0;
    }

    initialize();
}

void ENC_Task(void)
{
    packetReceive();
    g_enc28_erxfcon = readRegByte(ERXFCON);
}

bool ENC_getnewentry(uint8_t * buffer)
{
    bool result = false;
    if( nextrxslot != latestreadslot ){
        result = true;
        memcpy( buffer, rxbuffer[latestreadslot], rxbuffersize[latestreadslot]);
        latestreadslot++;
        latestreadslot = latestreadslot >= MAX_RX_BUFFER_N ? 0 : latestreadslot;
    }
    return result;
}

uint8_t readOp (uint32_t op, uint32_t address)
{
    uint8_t result;
    uint8_t spicmd_tx[2];
    uint8_t spicmd_rx[2];
    if (address & 0x80) {
        spicmd_tx[0] = op | ((address) & ADDR_MASK);
        spicmd_tx[1] = 0x00;
        spicmd_tx[2] = 0x00;
        while(SPI_sync_transmission(3, spicmd_tx, spicmd_rx) == false){continue;}
        result = spicmd_rx[2];
    }else{
        spicmd_tx[0] = op | (address & ADDR_MASK);
        spicmd_tx[1] = 0x00;
        while(SPI_sync_transmission(2, spicmd_tx, spicmd_rx) == false){continue;}
        result = spicmd_rx[1];
    }
    return result;
}

void writeOp (uint32_t op, uint32_t address, uint32_t data)
{
    uint8_t spicmd_tx[2];
    uint8_t spicmd_rx[2];
    spicmd_tx[0] = op | (address & ADDR_MASK);
    spicmd_tx[1] = (uint8_t)data;
    while(SPI_sync_transmission(2, spicmd_tx, spicmd_rx) == false){continue;}
}

void readBuf(uint16_t len, uint8_t *data)
{
    uint8_t spicmd_tx[16];
    for(uint16_t i = 0; i < len; i+=15){
        uint16_t remainingbytes = i + 15 > len ? len - i + 1 : 16;
        spicmd_tx[0] = ENC28J60_READ_BUF_MEM;
        spicmd_tx[1] = 0x00;
        while(SPI_sync_transmission( remainingbytes, spicmd_tx, (uint8_t*)(data + i )) == false){continue;}
        for(uint8_t j = 0; j < remainingbytes; j++){
            data[ i + j ] = data[ i + j + 1];
        }
        delay_us(100);
    }
}

void writeBuf(uint16_t len, const uint8_t *data)
{
    uint8_t spicmd_tx[16];
    uint8_t spicmd_rx[16];
    for(uint16_t i = 0; i < len; i+=15){
        uint16_t remainingbytes = i + 15 > len ? len - i + 1 : 16;
        spicmd_tx[0] = ENC28J60_WRITE_BUF_MEM;
        memcpy( spicmd_tx + 1, data + i, remainingbytes - 1);
        while(SPI_sync_transmission(remainingbytes, spicmd_tx, spicmd_rx) == false){continue;}
    }
}

void SetBank (uint32_t address)
{
    if ((address & BANK_MASK) != Enc28j60Bank) {
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank >> 5);
    }
}

uint32_t readRegByte (uint32_t address)
{
    SetBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

uint16_t readReg(uint32_t address)
{
    return readRegByte(address) + (readRegByte(address + 1) << 8);
}

void writeRegByte (uint32_t address, uint32_t data)
{
    SetBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void writeReg(uint32_t address, uint16_t data)
{
    writeRegByte(address, data);
    writeRegByte(address + 1, data >> 8);
}

uint16_t readPhyByte (uint32_t address)
{
    writeRegByte(MIREGADR, address);
    writeRegByte(MICMD, MICMD_MIIRD);
    while (readRegByte(MISTAT) & MISTAT_BUSY){ continue;}
    writeRegByte(MICMD, 0x00);
    return readRegByte(MIRD + 1);
}

void writePhy (uint32_t address, uint16_t data)
{
    writeRegByte(MIREGADR, address);
    writeReg(MIWR, data);
    while (readRegByte(MISTAT) & MISTAT_BUSY){ continue; }
}

void initialize (void)
{
    writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    delay_ms(4);
    while (!(readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY)){continue;}
    writeReg(ERXST, RXSTART_INIT);
    writeReg(ERXRDPT, RXSTOP_INIT);
    writeReg(ERXND, RXSTOP_INIT);
    writeReg(ERDPT, RXSTART_INIT);
    writeReg(ETXST, TXSTART_INIT);
    writeReg(ETXND, TXSTOP_INIT);
    writePhy(PHLCON, 0x476);
    #if 0
    // Rx filter: only unicast messages
    writeRegByte(ERXFCON, ERXFCON_UCEN | ERXFCON_ANDOR);
    #else
    // Rx filter: both unicast and broadcast messages
    writeRegByte(ERXFCON, ERXFCON_UCEN | ERXFCON_BCEN);
    #endif
    writeReg(EPMM0, 0x303f);
    writeReg(EPMCS, 0xf7f9);
    writeRegByte(MACON1, MACON1_MARXEN);
    writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
        MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    writeReg(MAIPG, 0x0C12);
    writeRegByte(MABBIPG, 0x12);
    writeReg(MAMXFL, MAX_FRAMELEN);
    writeRegByte(MAADR5, macaddr[0]);
    writeRegByte(MAADR4, macaddr[1]);
    writeRegByte(MAADR3, macaddr[2]);
    writeRegByte(MAADR2, macaddr[3]);
    writeRegByte(MAADR1, macaddr[4]);
    writeRegByte(MAADR0, macaddr[5]);
    readRegByte(MAADR5);
    readRegByte(MAADR4);
    readRegByte(MAADR3);
    readRegByte(MAADR2);
    readRegByte(MAADR1);
    readRegByte(MAADR0);
    writePhy(PHCON2, PHCON2_HDLDIS);
    SetBank(ECON1);
    writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
    _enc28_rev = readRegByte(EREVID);
    g_enc28_erxfcon = readRegByte(ERXFCON);
}

bool isLinkUp()
{
    uint8_t result = readPhyByte(PHSTAT2);
    bool isup = (result & 0x04) != 0;
    return isup;
}



void packetSend(uint16_t len, const uint8_t * buffer)
{
    uint16_t count = 0;
    uint32_t retry = 0;
    uint32_t eir_stat = readRegByte(ESTAT);
    uint32_t eir_reg;
    while (retry < 10) {
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
        writeOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);
        // prepare new transmission
        if (retry == 0) {
            writeReg(EWRPT, TXSTART_INIT);
            writeReg(ETXND, TXSTART_INIT + len);
            writeOp(ENC28J60_WRITE_BUF_MEM, 0, 0x02);
            eir_stat = readRegByte(ESTAT);
            writeBuf(len, buffer);
            eir_stat = readRegByte(ESTAT);
        }
        // initiate transmission
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
        eir_reg = readRegByte(EIR);
        while ((eir_reg & (EIR_TXIF | EIR_TXERIF)) == 0){
            count++;
            delay_us(10);
            if( count > 1000){
                transmit_status_vector_st tsv;
                writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
                uint16_t etxnd = readReg(ETXND);
                writeReg(ERDPT, etxnd + 1);
                readBuf(sizeof(tsv), (uint8_t *) &tsv);
                if (!((readRegByte(EIR) & EIR_TXERIF) &&
                        (tsv.ts[3] & 1 << 5) )){
                    retry = 10;
                }else{
                    retry++;
                }
                break;
            }
            eir_reg = readRegByte(EIR);
            eir_stat = readRegByte(ESTAT);
        }
        if ((eir_reg & EIR_TXIF ) != 0){
            break;
        }
    }
}


uint16_t packetReceive(void)
{
    static uint16_t gNextPacketPtr = RXSTART_INIT;
    uint16_t len = 0;
    while (readRegByte(EPKTCNT) > 0) {
        writeReg(ERDPT, gNextPacketPtr);
        struct  {
            uint16_t nextPacket;
            uint16_t uint32_tCount;
            uint16_t status;
        } header;
        readBuf(sizeof header, (uint8_t *) &header);
        len = header.uint32_tCount - 4; //remove the CRC count
        len = (len > (ENC_MAX_BUF_SIZE - 1)) ? (ENC_MAX_BUF_SIZE - 1) : len;
        if ((header.status & 0x80) == 0) {
            len = 0;
        } else {
            readBuf(len, rxbuffer[nextrxslot]);
            rxbuffersize[nextrxslot] = len;
            nextrxslot++;
            nextrxslot = nextrxslot >= MAX_RX_BUFFER_N ? 0 : nextrxslot;
        }
        writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
        if (gNextPacketPtr == 0) {
            writeReg(ERXRDPT, RXSTOP_INIT);
        } else {
            writeReg(ERXRDPT, gNextPacketPtr - 1);
        }
        gNextPacketPtr  = header.nextPacket;
    }
    return len;
}


void powerDown()
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
    while (readRegByte(ESTAT) & ESTAT_RXBUSY){continue;}
    while (readRegByte(ECON1) & ECON1_TXRTS){continue;}
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);
}

void powerUp()
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
    while (!(readRegByte(ESTAT) & ESTAT_CLKRDY)){continue;}
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

void enableBroadcast (void)
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) | ERXFCON_BCEN);
}

void disableBroadcast (void)
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) & ~ERXFCON_BCEN);
}

void enableMulticast ()
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) | ERXFCON_MCEN);
}

void disableMulticast ()
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) & ~ERXFCON_MCEN);
}
