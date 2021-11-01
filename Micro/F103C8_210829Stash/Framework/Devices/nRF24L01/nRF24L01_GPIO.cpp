/*
 * nRF24L01_GPIO.cpp
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#include "nRF24L01_GPIO.h"


NRF24L01_GPIO::NRF24L01_GPIO(
		GPIO_TypeDef* CE_GPIO_Port, uint16_t CE_Pin,
		GPIO_TypeDef* CSN_GPIO_Port, uint16_t CSN_Pin,
		GPIO_TypeDef* IRQ_GPIO_Port, uint16_t IRQ_Pin 	)
{
	this->CE_GPIO_Port 	= CE_GPIO_Port;
	this->CE_Pin 		= CE_Pin;
	this->CSN_GPIO_Port = CSN_GPIO_Port;
	this->CSN_Pin 		= CSN_Pin;
	this->IRQ_GPIO_Port = IRQ_GPIO_Port;
	this->IRQ_Pin		= IRQ_Pin;
}

