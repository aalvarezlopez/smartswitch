#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

char mock_cdc_buff[64];
uint16_t mock_cdc_buff_len;
char mock_uart_buff[64];
uint16_t mock_uart_buff_len;
char mock_rxuart_buff[64];
uint16_t mock_rxuart_buff_len;
char mok_uart_rx[128];
bool mock_lights = false;

void BCD_TO_INT(void)
{
}

void Display_printDate(void)
{
}

void Display_printHeat(void)
{
}

void Display_printTemp(void)
{
}

void Display_printTime(void)
{
}

void Display_refresh(void)
{
}

void IO_getLastAcquiredValue(void)
{
}

bool IO_getLights(void)
{
    return mock_lights;
}

void IO_getRadiatorState(void)
{
}

void IO_getShutterPosition(void)
{
}

void IO_openRadiatorValve(void)
{
}

void IO_setDimmer(void)
{
}

void IO_setLights(bool value)
{
    mock_lights = value;
}

void IO_setShutterPosition(void)
{
}

void Rtc_getTimeHour(void)
{
}

void Rtc_setTimeDate(void)
{
}

void delay_ms(void)
{
}

void udi_cdc_is_rx_ready(void)
{
}

void IO_isPIRactive(void)
{
}

void DS18B20_getTempC(void)
{
}

void Rtc_getTimeDate(void)
{
}

void UART_tx(const uint8_t* const dout, uint8_t len)
{
    memcpy(mock_uart_buff, dout, len);
    mock_uart_buff_len = len;
}

void udi_cdc_getc(void)
{
}

uint16_t udi_cdc_write_buf(const void* buf, uint16_t size)
{
    memcpy(mock_cdc_buff, buf, size);
    mock_cdc_buff_len = size;
}

void NvM_ReadBlock(void)
{
}
void NvM_Write(void)
{
}

uint8_t UART_flush(char * const ch)
{
    uint8_t len = strlen(mok_uart_rx);
    strcpy(ch, mok_uart_rx);
    memset(mok_uart_rx, 0, 128);
    return len;
}
