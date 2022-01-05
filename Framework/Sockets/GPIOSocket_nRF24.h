/*
 * nRF24L01_GPIO.h
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#ifndef DEVICES_NRF24L01_NRF24L01_GPIO_H_
#define DEVICES_NRF24L01_NRF24L01_GPIO_H_

#ifdef __x86_64
#include "stm32f1xx_hal.h"
#include <X86Tasks/SimulationTask.h>
#elif defined STM32F401xE
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#endif
#include <Config/config.h>

#ifndef __x86_64
// FIXME make it working on X86
class GPIOSocket_nRF24 {
public:
	GPIOSocket_nRF24() {
	}
	virtual ~GPIOSocket_nRF24() {
	}
	inline void nRF24_CE_L(void) {
		HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
	}
	inline void nRF24_CE_H(void) {
		HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);
	}
	inline void nRF24_CSN_L(void) {
		HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
	}
	inline void nRF24_CSN_H(void) {
		HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
	}
	static inline uint16_t get_IRQ_Pin(void) {
		return NRF_IRQ_Pin;
	}

private:

};

#endif
#endif /* DEVICES_NRF24L01_NRF24L01_GPIO_H_ */
