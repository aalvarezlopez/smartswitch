/*
 * spisw.h
 *
 *  Created on: 03/12/2013
 *      Author: JRB
 */

#ifndef SPISW_H_
#define SPISW_H_

void SpiSWConfigure(uint8_t my_cpol, uint8_t my_cpha);
uint8_t SpiSWTransfer(uint8_t data);
void SpiSWInit();

#endif /* SPISW_H_ */
