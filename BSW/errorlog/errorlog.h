/**
 * @file errorlog.h
 * @brief Keeps all the error under the same component
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-07-02
 */

#define MAX_HISTORY_LEN 10u

typedef enum{
	SPI_MODULE = 10,
	DS18B20_MODULE = 11,
}MODULES_t;

void errorlog_Init(void);
void errorlog_reportError(MODULES_t code, uint8_t *info, uint8_t len);
