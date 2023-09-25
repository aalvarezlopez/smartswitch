#include "stdio.h"
#include "display.h"
#include "display_pri.h"
#include "delays.h"
#include "graphics.h"

#include "i2c.h"

uint8_t displayBuffer[DISPLAY_PAGES][DISPLAY_WIDTH];

void Display_Init(void)
{
    uint8_t data[2];
    for(uint8_t i = 0; i < DISPLAY_PAGES; i++){ 
        memset(&(displayBuffer[i][0]), 0x0, DISPLAY_WIDTH);
    }

    display_command( DISPLAYOFF, NULL, 0);

    display_command(DISPLAYON, NULL, 0);

    showWelcome();
}

void display_command( display_cmd_et cmd, const uint8_t * const data, uint8_t len)
{
    uint8_t out[10];
    uint8_t size;
    out[1] = cmd;
    size = 2;
    if( len > 1){
        out[0] = 0x80;          /* C0 = 0; this is the last command
                                   D/~C = 0; Command operation */
        for(uint8_t i = 0; i < len; i++){
            out[2 + (i * 2)] = (i + 1) >=len ? 0x00 : 0x80;
            out[3 + (i * 2)] = data[i];
            size +=2;
        }
    }else{
        out[0] = 0x00;          /* C0 = 0; this is the last command
                                   D/~C = 0; Command operation */
    }
    I2C_sync_transmission( out, size);
}

void display_writeData(const uint8_t * data, uint8_t len)
{
    uint8_t out[24];
    out[0] = 0x40;
    memcpy(out + 1, data, len);
    I2C_sync_transmission( out, len + 1);
}

void showWelcome(void)
{
    uint8_t data[2];
    uint8_t dout[1024];
    gpx_putString("23/09", 48, 56);
    gpx_putLargeDigit('1', 10, 0);
    gpx_putLargeDigit('1', 36, 0);
    displayBuffer[1][63] = 0x3C; 
    displayBuffer[1][64] = 0x3C; 
    displayBuffer[1][65] = 0x3C;
    displayBuffer[2][63] = 0x3C; 
    displayBuffer[2][64] = 0x3C; 
    displayBuffer[2][65] = 0x3C;
    gpx_putLargeDigit('4', 68, 0);
    gpx_putLargeDigit('8', 94, 0);
    gpx_putSmallDigit('2', 2, 34);
    gpx_putSmallDigit('7', 20, 34);
    gpx_putSmallDigit('2', 94, 34);
    gpx_putSmallDigit('2', 112, 34);
    for(uint8_t page = 0; page < DISPLAY_PAGES; page++){
        display_command(0xB0 + page, NULL, 0);
        display_command(0x02, NULL, 0);
        display_command(0x10, NULL, 0);

        for(uint8_t i = 0; i < DISPLAY_WIDTH/DISPLAY_DATA_STREAM_LEN; i++){
            display_writeData(&(displayBuffer[page][DISPLAY_DATA_STREAM_LEN * i]) , DISPLAY_DATA_STREAM_LEN);
        }
    }
}
