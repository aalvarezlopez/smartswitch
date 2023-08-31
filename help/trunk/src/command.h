/// command.h
///
/// Copyright (C) 2013 INGEN10 Ingenieria SL
/// http://www.ingen10.com
///
/// LEGAL NOTICE:
/// All information contained herein is, and remains property of INGEN10 Ingenieria SL.
/// Dissemination of this information or reproduction of this material is strictly
/// forbidden unless prior written permission is obtained from its owner.
/// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC PERFORMANCE, OR PUBLIC DISPLAY
/// OF, OR THROUGH USE OF THIS SOURCE CODE IS STRICTLY PROHIBITED, AND IT IS A VIOLATION
/// OF INTERNATIONAL TRADE TREATIES AND LAWS.
/// THE RECEIPT OR POSSESSION OF THIS DOCUMENT DOES NOT CONVEY OR IMPLY ANY RIGHTS.
///
/// Authored by:   AV (12/12/2013)
/// Revised by:
/// Last Version:
///
/// FILE CONTENTS:
/// command.h functions to parse commands

#ifndef COMMAND_H_
#define COMMAND_H_

#include "params.h"

#define INITC '>'
#define RESPC '<'
#define ENDC ';'
#define ACKC "ACK_"
#define NACKC "NACK"
#define MAX_BUFFER_SIZE 20
#define END_STRING_CHAR '\0'
#define EQUAL_CHAR '='
#define DATA_READY 1
#define PRINT_ERROR 2
#define BLANK_CHAR '0'

#define RUN "run"
#define STOP "stop"
#define TIME "time"
#define TSTAB "tstab"
#define NSTAB "nstab"
#define DATE "date"
#define LWAKEUP "lwakeup"
#define LSTAB "lstab"
#define TMEAS "tmeas"
#define TDELAY "tdelay"
#define TOFF "toff"
// REVIEW: AAL SRATE already defined in status_machine.h
#define SRATE "srate"
#define ID "id"
#define GAIN "gain"
#define ACEL "acel"
#define GETBAT "getbat"
#define PGA "pga"
#define CALIB "calibration"
#define READADC "readadc"
#define CSHUTDOWN "shutdown"
#define UPDATE "update"
#define REBOOT "reboot"
#define CALIBRATE "calibrate"

int ReturnError();
void ReturnResponse(char* command);
void ReturnResponseGet(char* command);
int ProcessIncomingRda(Params* my_params);
int ParseCommand(Params* myParams);
int GetCommand(char* command, Params* myParams);
void IncomingRda(char* text, int size);

#endif /* COMMAND_H_ */
