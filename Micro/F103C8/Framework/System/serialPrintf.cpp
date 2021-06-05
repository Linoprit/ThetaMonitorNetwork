/*
 * uart_messaging.c
 *
 *  Created on: 16.12.2018
 *      Author: harald
 */

#include <System/OsHelpers.h>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>
#include <Instances/config.h>
#include "libraries/HelpersLib.h"
#include <System/serialPrintf.h>

// SERIAL_UART must be defined in config.h

constexpr uint16_t TX_BUFF_LEN = 256;
constexpr uint8_t TX_DELAY = 10;
constexpr uint8_t TX_RETRIES = 10;

uint8_t txBuff[TX_BUFF_LEN];
uint16_t tx_act_pos = 0;

// private
void tx_buff_clear(void);

/*
 * to enable FLOAT formatted output, see:
 * https://stackoverflow.com/questions/28334435/stm32-printf-float-variable.
 * - add -u _printf_float to your LDFLAGS.
 * or
 * - printf("Result is: %d.%d", i/10, i%10);
 */
int tx_printf(const char *format, ...) {
	va_list arg;
	va_start(arg, format);
	tx_act_pos += vsprintf((char*) &txBuff[tx_act_pos], format, arg);
	va_end(arg);

	return _SUCCESS;
}

uint8_t tx_cycle(void) {

	if (tx_act_pos > 0) {
		uint8_t result = _ERROR;

		for (uint8_t i = 0; i < TX_RETRIES; i++) {

			//result = CDC_Transmit_FS(txBuff, tx_act_pos);
			result = HAL_UART_Transmit_IT(&SERIAL_UART, &txBuff[0], tx_act_pos);
			//result = HAL_UART_Transmit(deb_huart, &txBuff[0], tx_act_pos, 100);

			// @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
			if (result == _SUCCESS) {
				break;
			} else {
				OsHelpers::delay(TX_DELAY);
			}
		}

		// buffer is in transmission, we must not clear it!
		tx_act_pos = 0;
		return result;
	}
	return _SUCCESS;
}

/*
 * directly print buffer, without formatting
 */
int tx_printBuff(uint8_t *buffer, uint8_t len) {
	memcpy(&txBuff[tx_act_pos], buffer, (std::size_t) len);
	tx_act_pos += len;
	return _SUCCESS;
}

void tx_buff_clear(void) {
	memset(txBuff, '\0', TX_BUFF_LEN);
	tx_act_pos = 0;
}

