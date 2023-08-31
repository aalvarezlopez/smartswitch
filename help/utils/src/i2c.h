/*
 * i2c.h
 *
 *  Created on: 10/12/2013
 *      Author: Adrian
 */

#ifndef I2C_H_
#define I2C_H_

#define I2CTIMEOUT 1000

int I2CReadData(unsigned int address, unsigned int* data, unsigned int nBytes);
int I2CWriteData(unsigned int address, const unsigned int* data,
                 unsigned int nBytes);
int I2CWriteReadData(unsigned int address, const unsigned int* dataToSend,
                     int bytesToSend, unsigned int* dataRead, unsigned int bytesToRead);
void EnableI2C(unsigned int clockDivisor);
int CheckDevice(unsigned int address);

#endif /* I2C_H_ */
