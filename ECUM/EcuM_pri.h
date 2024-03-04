/**
 * @file EcuM_pri.h
 * @brief Private functions and variables
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */

#ifndef ECUM_PRI_H
#define ECUM_PRI_H

void EcuM_Startup_one(void);
void ecum_configure_io_interfaces(void);
void ecum_configure_spi_interface(void);
void ecum_configure_pwm_interface();
void ecum_configure_uart_interface(void);
void ecum_configure_analog_input_interface(void);
void ecum_configure_peripheral_clocks(void);
uint32_t ecum_TestFlash(void);

#endif
