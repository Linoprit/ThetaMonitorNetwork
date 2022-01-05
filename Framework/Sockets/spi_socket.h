/*
 * spi_socket.h
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#ifndef SOCKETS_SPI_SOCKET_H_
#define SOCKETS_SPI_SOCKET_H_

#ifdef __x86_64
#include "stm32f1xx_hal.h"
#elif defined STM32F401xE  || defined STM32F401xC
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"
#endif

#ifndef __x86_64
// FIXME make it working on X86
class Spi_socket {
public:
	Spi_socket() :
			hspi { nullptr } {
		// NOP
	}
	;
	Spi_socket(SPI_HandleTypeDef *hspi) {
		this->hspi = hspi;
		this->rx_buffer = 0;
	}
	;
	virtual ~Spi_socket() {
	}
	;
	void setSpi(SPI_HandleTypeDef *hspi) {
		this->hspi = hspi;
	}
	;
	uint8_t readwrite(uint8_t data) {
		// TODO blocking...
		while (HAL_SPI_GetState(this->hspi) != HAL_SPI_STATE_READY) {
		}

		HAL_SPI_TransmitReceive(this->hspi, &data, &rx_buffer, 1, 500);
		return rx_buffer;
	}
private:
	SPI_HandleTypeDef *hspi;
	uint8_t rx_buffer; // TODO reads only one byte...
};
#endif

#endif /* SOCKETS_SPI_SOCKET_H_ */
