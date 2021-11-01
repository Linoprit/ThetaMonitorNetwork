/**
 * \file            ow_ll_template.c
 * \brief           Low-level UART communication
 */
 
/*
 * Copyright (c) 2018 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of OneWire-UART library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */


#include "stm32f1xx_hal.h"
#include "ow_ll.h"
#include <main.h>
#include <System/serialPrintf.h>


extern UART_HandleTypeDef huart3;
UART_HandleTypeDef* ow_huart = &huart3;
//extern DMA_HandleTypeDef*  get_hdma_usart3_rx(void);
#define UART_PORT ow_huart
//#define UART_DMA_RX get_hdma_usart3_rx()

#define UART_TX_PORT CH1_TX_GPIO_Port
#define UART_TX_PIN CH1_TX_Pin



/**
 * \brief           Initialize low-level communication
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t ow_ll_init(void* arg)
{
	/* Configure and enable UART at 115200 bauds */

	/*Configure UART GPIO pin open-drain */
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = UART_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(UART_TX_PORT, &GPIO_InitStruct);

	return 1;
}

/**
 * \brief           Deinit low-level
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t ow_ll_deinit(void* arg)
{
    /* Disable UART peripheral */

	//HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)


    return 1;
}

/**
 * \brief           Set baudrate for UART hardware
 * \param[in]       baud: Expected baudrate for UART. Values can be either `9600` or `15200` bauds
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t ow_ll_set_baudrate(uint32_t baud, void* arg)
{
	/* Configure UART to selected baudrate */
	UART_PORT->Init.BaudRate = baud;
	HAL_UART_Init(UART_PORT);

	return 1;
}


/**
 * \brief           Transmit-Receive data over UART
 * \param[in]       tx: Array of data to send
 * \param[out]      rx: Array to save receive data 
 * \param[in]       len: Number of bytes to send
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t ow_ll_transmit_receive(
		const uint8_t* tx, uint8_t* rx, size_t len, void* arg)
{
	/* Perform data exchange */
	uint32_t timeout = 0x00FF;
	//uint32_t count   = 0;

	if (len <= 0 )
		return 0;

	/*HAL_UART_Transmit_DMA(UART_PORT, (uint8_t*) tx, len);
	HAL_UART_Receive_DMA(UART_PORT, (uint8_t*) rx, len);

	// wait until USART has finished. UNSAFE - hangs often
	while(HAL_DMA_GetState(UART_DMA_RX) == HAL_DMA_STATE_BUSY)
	{
		osDelay(1); // use osdelay() in OS environment here
		count++;
		if (count >= timeout)
		{
			return 0;
		}
	}	*/

	// Byte for byte version
	while (len--)
	{
		HAL_UART_Transmit(UART_PORT, (uint8_t*) tx, 1, timeout);
		tx++;
		HAL_UART_Receive(UART_PORT, rx, 1, timeout);
		rx++;
	}

    return 1;
}

