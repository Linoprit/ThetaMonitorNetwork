/*
 * serialPrintf.cpp
 *
 *  Created on: 21.03.2021
 *      Author: harald
 */

#include "serialPrintf.h"
#include <Types/AppTypes.h>

//int tx_printBuff(uint8_t* buffer, uint8_t len) {
//	uint8_t outBuff[len+1];
//	memcpy(outBuff, buffer, len);
//	outBuff[len+1] = '\0';
//	tx_printf("%s", outBuff);
//	return 0;
//}
//
//int tx_cycle(void) {
//	// only needed on micros, nothing to do here.
//	return SUCCESS;
//}


#include <System/OsHelpers.h>
#include <cstring>
#include <stdio.h>
//#include <stdarg.h>
#include "Libraries/HelpersLib.h"
//#include <System/serialPrintf.h>
#include <SerialIO/SerialIO.h>
#include "printf.h"

// SERIAL_UART must be defined in config.h

constexpr uint16_t TX_BUFF_LEN = 512;
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
//int tx_printf(const char *format, ...) {
	//va_list arg;
	//va_start(arg, format);
	//tx_act_pos += vsprintf((char*) &txBuff[tx_act_pos], format, arg);
	////tx_act_pos += snprintf((char*) &txBuff[tx_act_pos], tx_buff_clear, format, arg);
	//va_end(arg);
//	return SUCCESS;
//}

void _putchar(char character){
	 txBuff[tx_act_pos] = character;
	 tx_act_pos++;
	 if(tx_act_pos >= (TX_BUFF_LEN-1)){
		 tx_act_pos = TX_BUFF_LEN-1;
	 }
}

uint16_t tx_free_bytes(void) { return TX_BUFF_LEN - tx_act_pos; }


int tx_cycle(SerialIO* serialIO) {

	if (tx_act_pos > 0) {
		uint8_t result = FAIL;

		for (uint8_t i = 0; i < TX_RETRIES; i++) {

			//result = CDC_Transmit_FS(txBuff, tx_act_pos);
			//result = HAL_UART_Transmit_IT(&SERIAL_UART, &txBuff[0], tx_act_pos);
			//result = HAL_UART_Transmit(deb_huart, &txBuff[0], tx_act_pos, 100);
			result = serialIO->transmit(txBuff, tx_act_pos);

			// @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
			if (result == SUCCESS) {
				break;
			} else {
				OsHelpers::delay(TX_DELAY);
			}
		}

		// buffer is in transmission, we must not clear it!
		tx_act_pos = 0;
		return result;
	}
	return SUCCESS;
}

/*
 * directly print buffer, without formatting
 */
int tx_printBuff(uint8_t *buffer, uint8_t len) {
	memcpy(&txBuff[tx_act_pos], buffer, (std::size_t) len);
	tx_act_pos += len;
	return SUCCESS;
}

void tx_buff_clear(void) {
	memset(txBuff, '\0', TX_BUFF_LEN);
	tx_act_pos = 0;
}
