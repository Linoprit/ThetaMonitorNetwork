/*
 * uart_messaging.h
 *
 *  Created on: 16.12.2018
 *      Author: harald
 */

#ifndef UART_PRINTF_H_
#define UART_PRINTF_H_

//#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

//EXTERNC int tx_printf(const char *format, ...);
EXTERNC int tx_printBuff(uint8_t* buffer, uint8_t len) ;
EXTERNC void _putchar(char character);
EXTERNC uint16_t tx_free_bytes(void);

//EXTERNC int tx_cycle(SerialIO* serialIO);
//EXTERNC void tx_buff_clear(void);

#undef EXTERNC
#ifdef __cplusplus
// put cpp includes here!!

//int tx_printf(const char *format, ...);
int tx_printBuff(uint8_t* buffer, uint8_t len) ;
//int tx_cycle(SerialIO* serialIO);
uint16_t tx_free_bytes(void);

#endif // C interface
#endif /* UART_PRINTF_H_ */
