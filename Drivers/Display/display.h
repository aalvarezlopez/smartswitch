#include "stdbool.h"

#ifndef DISPLAY_H
#define DISPLAY_H


#define DISPLAY_PAGE_SIZE 8u
#define DISPLAY_WIDTH 128u
#define DISPLAY_HEIGHT 64u
#define DISPLAY_PAGES 8u
#define DISPLAY_DATA_STREAM_LEN 16u

extern uint8_t displayBuffer[][DISPLAY_WIDTH];

void Display_Init(void);
void Display_printHeat( bool state );
void Display_printTemp( int16_t internal, int16_t target);
void Display_printDate( uint8_t day, uint8_t month);
void Display_printTime( uint8_t hour, uint8_t minutes);
void Display_refresh(void);

#endif
