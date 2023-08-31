/*
 * status_machine.h
 *
 *  Created on: 17/12/2013
 *      Author: Adrian
 */

#ifndef STATUS_MACHINE_H_
#define STATUS_MACHINE_H_

#include "params.h"

enum STATUS {
    SHUTDOWN, STAND_BY, READY, DELAY_ARMED, MEASURING, STOPPING, DELAY_OFF, IDLE
};

#define true 1
#define false 0
#define BUFFERSIZE 2500
#define NBYTESTOWRITE 100
#define MAXSRATE 12800

#define ARMED_NSECS     1       //
#define STANDBYTICKS    4


void InitStatusMachine(Params* p_params);
int StatusMachine();

void ExecuteStop();
void ExecuteRun();
void InitWavFile();

#endif /* STATUS_MACHINE_H_ */
