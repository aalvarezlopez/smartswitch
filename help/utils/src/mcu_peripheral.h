/*
 * board_init.h
 *
 *  Created on: Dec 2, 2013
 *      Author: Adrian
 */

#ifndef BOARD_INIT_H_
#define BOARD_INIT_H_

#define PULLUPENABLE    1
#define PULLDISABLE     0
#define OUTPUTPIN   0
#define INPUTPIN    1

#define FUNCTION_A 0
#define FUNCTION_B 1
#define FUNCTION_C 2
#define FUNCTION_D 3
#define FUNCTION_E 4
#define PMR0_MASK   1
#define PMR1_MASK   (1 << 1)
#define PMR2_MASK   (1 << 2)

#define HIGH 1
#define LOW 0

#define PLL0    0
#define OSCCTRLWORD 0x00010307
#define PLL0CONFIGWORD  0x3F0F0401
#define WAITSATE 0x40
#define SYSCLK_SRC_PLL0 0

#define TIMER0 0
#define TIMER_CLOCK1 0
#define TIMER_CLOCK2 1
#define TIMER_CLOCK3 2
#define TIMER_CLOCK4 3
#define TIMER_CLOCK5 4

#define TMRINTENABLE    1
#define TMRINTDISABLE   0

#define AVDD_ON     1
#define AVDD_OFF    0

#define EIC_INT_ON   1
#define EIC_INT_OFF  0

void ConfigureOsc();
void TimerEnableInterrupt(int channel, int enable);
void SpiCSDConfiguration(int channel, int baudrate);
void TogglePinValue(int pin);
void SetPinValue(int pin, int value);
void DisablePin(int pin);
void SetPinFunction(int pin, int function);
void SetPinDirection(int pin, int direction, int pullUpEnable);
void TimerCaptureConfigure(int channel, int clock, int captureReg,
                           int priorityInt);
void ExternalInterruptConfigure(int pin, int external_interrupt,
                                int function, int priority);
void EICEnable(int external_interrupt, int enable);
void EICHandlerEnable(int external_interrupt, int enable);

#endif /* BOARD_INIT_H_ */
