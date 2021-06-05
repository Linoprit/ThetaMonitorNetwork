/*
 * spi_socket.h
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#ifndef SOCKETS_SPI_SOCKET_H_
#define SOCKETS_SPI_SOCKET_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"



class Spi_socket {
public:
	Spi_socket(SPI_HandleTypeDef* hspi);
	virtual ~Spi_socket() {};

	uint8_t readwrite(uint8_t data);


private:
	SPI_HandleTypeDef* 	hspi;
	uint8_t 			rx_buffer;

};

#endif /* SOCKETS_SPI_SOCKET_H_ */
