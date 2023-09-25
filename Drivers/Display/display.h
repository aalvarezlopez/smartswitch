#ifndef DISPLAY_H
#define DISPLAY_H


#define DISPLAY_PAGE_SIZE 8u
#define DISPLAY_WIDTH 128u
#define DISPLAY_HEIGHT 64u
#define DISPLAY_PAGES 8u
#define DISPLAY_DATA_STREAM_LEN 16u

extern uint8_t displayBuffer[][DISPLAY_WIDTH];

void Display_Init(void);

#endif
