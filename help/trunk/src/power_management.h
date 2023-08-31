/*
 * power_management.h
 *
 *  Created on: 21/12/2013
 *      Author: JRB
 */

#ifndef POWER_H_
#define POWER_H_

#ifndef ON
    #define ON 1
#endif
#ifndef OFF
    #define OFF 0
#endif

enum LEVELS {
    WARNING_LEVEL,
    CRITICAL_LEVEL,
    NORMAL_LEVEL
};

void Shutdown();
void SetMcuLowPower();
void SetMcuFullPower();
void ConfigureBackUpMode();

unsigned int ReadBatteryLevel();
void GetBatteryLevel();
void UpdateBatteryLevel(unsigned int mv);
int IsBatteryCritical();

void SetStatusLed(int state);
void BlinkStatusLed();
void ToggleStatusLed();

int IsUsbConnected();

void TurnOnAnalogDevices(int state);
void TurnOnSD(int state);
void SetCircuit(int state);

void ConfigureWdt();

void ResetWdt();

#endif /* POWER_H_ */
