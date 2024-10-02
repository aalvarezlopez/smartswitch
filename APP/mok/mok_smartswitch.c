#include "stdio.h"
#include "stdint.h"

char mock_cdc_buff[64];
uint16_t mock_cdc_buff_len;

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

void IO_getLights(void)
{
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

void IO_setLights(void)
{
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
}

void udi_cdc_getc(void)
{
}

uint16_t udi_cdc_write_buf(const void* buf, uint16_t size)
{
    memcpy(mock_cdc_buff, buf, size);
    mock_cdc_buff_len = size;
}

