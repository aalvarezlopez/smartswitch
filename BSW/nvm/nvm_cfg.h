/**
 * @file nvm_cfg.h
 * @brief Configure the NvM layout and scheme for each project
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-02-17
 */

#ifndef NVM_CFG_H
#define NVM_CFG_H

#ifdef __SAM4S4A__
#define NVM_VIRGIN_PATTERN 0xFF
#else
#define NVM_VIRGIN_PATTERN 0x00
#endif

/*   ---------
 *  12 bytes header
 *  4 bytes  ip
 *  2 bytes  id
 *  TOTAL: 18 bytes -> Writen in block of 8 bytes, 3 blocks (3*8=24 bytes)
 */
typedef struct nvmSettings_s{
	uint32_t header[3];
	uint8_t ip[4];
	uint16_t id;
}nvmSettings_st;

typedef struct partition_s{
    uint8_t numberOfSectors;   /*Number of NVM pages*/
#ifdef __SAM4S4A__
    uint32_t startofpartition;  /*Flash address where the block is starting*/
#else
    uint64_t startofpartition;  /*Emulated flash address */
#endif
    uint8_t sizeofblock;       /*Number of bytes of the data, including the sequence counter.
                                 It has to be divided by NVM_BLOCK_SIZE_BYTES*/
}partition_st;


#endif
