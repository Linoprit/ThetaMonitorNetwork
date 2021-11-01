/*
 * nRF24L01_GPIO.h
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#ifndef DEVICES_NRF24L01_NRF24L01_GPIO_H_
#define DEVICES_NRF24L01_NRF24L01_GPIO_H_

#ifndef __x86_64
// FIXME make it working on X86

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

class NRF24L01_GPIO
{
public:
	NRF24L01_GPIO(
			GPIO_TypeDef* CE_GPIO_Port,  uint16_t CE_Pin,
			GPIO_TypeDef* CSN_GPIO_Port, uint16_t CSN_Pin,
			GPIO_TypeDef* IRQ_GPIO_Port, uint16_t IRQ_Pin );
	virtual ~NRF24L01_GPIO() {};

	inline void nRF24_CE_L(void) { HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET); }
	inline void nRF24_CE_H(void) { HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET); }

	inline void nRF24_CSN_L(void) { HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_RESET); }
	inline void nRF24_CSN_H(void) { HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_SET); }

	inline uint16_t get_IRQ_Pin(void) { return IRQ_Pin; }

private:

	GPIO_TypeDef* CE_GPIO_Port;
	GPIO_TypeDef* CSN_GPIO_Port;
	GPIO_TypeDef* IRQ_GPIO_Port;
	uint16_t CE_Pin;
	uint16_t CSN_Pin;
	uint16_t IRQ_Pin;
};

#endif
#endif /* DEVICES_NRF24L01_NRF24L01_GPIO_H_ */
