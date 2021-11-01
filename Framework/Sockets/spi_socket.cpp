/*
 * spi_socket.cpp
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#include "spi_socket.h"
#ifndef __x86_64
Spi_socket::Spi_socket(SPI_HandleTypeDef* hspi)
{
	this->hspi 		= hspi;
	this->rx_buffer = 0;
}


uint8_t Spi_socket::readwrite(uint8_t data)
{
	// TODO blocking...
	while(HAL_SPI_GetState(this->hspi) != HAL_SPI_STATE_READY)
	{
	}

	HAL_SPI_TransmitReceive(this->hspi, &data, &rx_buffer, 1, 500);
	return rx_buffer;
}
#endif
