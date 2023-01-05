/*
 * serialPrintf.cpp
 *
 *  Created on: 21.03.2021
 *      Author: harald
 */

#include "serialPrintf.h"
#include <Config/Types/AppTypes.h>
#include <stdarg.h>
#include <System/OsHelpers.h>
#include <cstring>
#include <stdio.h>
#include "Libraries/HelpersLib.h"
#include <System/serialPrintf.h>
#include <Config/config.h>

#ifdef __x86_64
#include <SerialIO/SerialIO.h>
#else
// ...
#endif

// SERIAL_UART must be defined in config.h
constexpr uint16_t TMP_BUFF_LEN = 128;
constexpr uint16_t TX_BUFF_LEN = 512;
constexpr uint8_t TX_DELAY = 10;
constexpr uint8_t TX_RETRIES = 3;

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
 * CAREFUL: the message must not be longer than TMP_BUFF_LEN!!
 */
int tx_printf(const char *format, ...) {
	uint8_t tmpBuff[TMP_BUFF_LEN];

	va_list arg;
	va_start(arg, format);
	uint16_t tmpLen = vsnprintf((char*) &tmpBuff, TX_BUFF_LEN, format, arg);
	va_end(arg);

	if (tx_free_bytes() < tmpLen)
		return _FAIL_;

	if(osSemaphoreAcquire(txPrintSemHandle, 10) != osOK){
		return _FAIL_;
	}
	std::memcpy(&txBuff[tx_act_pos], tmpBuff, tmpLen);
	tx_act_pos += tmpLen;
	osSemaphoreRelease(txPrintSemHandle);

	return _SUCCESS_;
}

void _putchar(char character){
	 txBuff[tx_act_pos] = character;
	 tx_act_pos++;
	 if(tx_act_pos >= (TX_BUFF_LEN-1)){
		 tx_act_pos = TX_BUFF_LEN-1;
	 }
}

uint16_t tx_free_bytes(void) { return TX_BUFF_LEN - tx_act_pos; }

#ifdef __x86_64
int tx_cycle(void){
	printf("%s", txBuff);
	tx_buff_clear();
	tx_act_pos = 0;
	return _SUCCESS_;
}

int tx_cycle(SerialIO* serialIO) {
#else
	int tx_cycle(void){
#endif

	if (tx_act_pos > 0) {
		uint8_t result = _FAIL_;

		for (uint8_t i = 0; i < TX_RETRIES; i++) {

#ifdef __x86_64
			result = serialIO->transmit(txBuff, tx_act_pos);
#else
			//result = CDC_Transmit_FS(txBuff, tx_act_pos);
			result = HAL_UART_Transmit_IT(&SERIAL_UART, &txBuff[0], tx_act_pos);
			//result = HAL_UART_Transmit_DMA(&SERIAL_UART, &txBuff[0], tx_act_pos);
			//result = HAL_UART_Transmit(&SERIAL_UART, &txBuff[0], tx_act_pos, 30);
#endif
			// @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY

			if (result == _SUCCESS_) {
				break;
			} else {
				OsHelpers::delay(TX_DELAY);
			}
		}

		// buffer is in transmission, we must not clear it!
		tx_act_pos = 0;
		return result;
	}
	return _SUCCESS_;
}

/*
 * directly print buffer, without formatting
 */
int tx_printBuff(uint8_t *buffer, uint8_t len) {
	if (tx_free_bytes() < len)
		return _FAIL_;

	if(osSemaphoreAcquire(txPrintSemHandle, 10) != osOK){
		return _FAIL_;
	}
	memcpy(&txBuff[tx_act_pos], buffer, (std::size_t) len);
	tx_act_pos += len;
	osSemaphoreRelease(txPrintSemHandle);
	return _SUCCESS_;
}

void tx_buff_clear(void) {
	if(osSemaphoreAcquire(txPrintSemHandle, 10) != osOK){
		return;
	}
	memset(txBuff, '\0', TX_BUFF_LEN);
	tx_act_pos = 0;
	osSemaphoreRelease(txPrintSemHandle);
}

// test memory consumption
// #include <stdio.h>
//int fputc(int ch, FILE *f)
//{
//  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
//  return ch;
//}
