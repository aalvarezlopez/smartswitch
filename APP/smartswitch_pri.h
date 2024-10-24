/**
 * @file smartswitch_pri.h
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-03-04
 */

#ifndef SMARTSWITCH_PRI_H
#define SMARTSWITCH_PRI_H

#define SMARTSWITCH_LIGHTSENSOR_ADC_CH 2u

/******** Darnkess level percentage function *****/

#define LOG_ACTION_BUTTON 
typedef enum smartswitchlog_e{
    BUTTON_ACTION,
    PRESENCE,
    BUTTON_EXT
}smartswitch_et;

#define SMARTSWITCH_DARK_0   1000
#define SMARTSWITCH_DARK_100 2200
#define PULSES_LITER 400u

void smartswitch_calculateflow(void);

#endif
