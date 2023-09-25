#ifndef DISPLAY_PRI_H
#define DISPLAY_PRI_H

typedef enum display_cmd_e{
    SETLOWCOLUMN = 0x00,
    EXTERNALVCC = 0x1,
    SWITCHCAPVCC = 0x2,
    SETHIGHCOLUMN = 0x10,
    COLUMNADDR = 0x21,
    PAGEADDR = 0x22,
    SETSTARTLINE = 0x40,
    SETCONTRAST = 0x81,
    CHARGEPUMP = 0x8D,
    SEGREMAP = 0xA0,
    DISPLAYALLON_RESUME = 0xA4,
    DISPLAYALLON = 0xA5,
    NORMALDISPLAY = 0xA6,
    INVERTDISPLAY = 0xA7,
    SETMULTIPLEX = 0xA8,
    DISPLAYOFF = 0xAE,
    DISPLAYON = 0xAF,
    COMSCANINC = 0xC0,
    COMSCANDEC = 0xC8,
    SETDISPLAYOFFSET = 0xD3,
    SETDISPLAYCLOCKDIV = 0xD5,
    SETCOMPINS = 0xDA,
    SETVCOMDETECT = 0xDB,
    SETPRECHARGE = 0xD9,
} display_cmd_et;

void display_command( display_cmd_et cmd, const uint8_t * const data, uint8_t len);
void showWelcome(void);
 #endif
