/**
 * @file nvm.h
 * @brief Header file of the nvm component
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-10-23
 */

#ifndef _NVM_H_
#define _NVM_H_

void NvM_Init(void);
uint8_t NvM_ReadAll(uint32_t *buffer);
void NvM_Write(uint32_t * buffer, uint8_t npart);
uint8_t NvM_GetSequence(uint8_t npart);
uint8_t NvM_ReadBlock(uint8_t npart, uint8_t blockid, uint8_t nentries, uint8_t *dout);

#endif
