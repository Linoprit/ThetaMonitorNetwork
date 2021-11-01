/*
 * callbacks.c
 *
 *  Created on: 16.05.2016
 *      Author: harald
 */

#include "callbacks.h"
#include <System/serialPrintf.h>
#include <Instances/config.h>
#include <System/CommandLine/CommandLine.h>

/*void HAL_SYSTICK_Callback(void){  }*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	UNUSED(GPIO_Pin);
}


uint8_t uart1Rx;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		cLine::CommandLine::instance().putChar(uart1Rx);
		HAL_UART_Receive_IT(&SERIAL_UART, &uart1Rx, 1);
	}

	//else if (huart->Instance == USART2) { 	}
	//else if (huart->Instance == USART3) {	}
}

// needed for dma-receive (extra stm32f1xx_hal_uart.* needed
//void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart) {
//	call_uart_callbacks(huart);
//}

// Workaround hanging usart-dma
//void USART1_IRQHandler(void)
//{
//	//HAL_GPIO_TogglePin(LED_03_GPIO_Port, LED_03_Pin);
//	HAL_UART_IRQHandler(&huart1);
//}
//void USART2_IRQHandler(void)
//{
//  HAL_UART_IRQHandler(get_huart2);
//}
//void USART3_IRQHandler(void)
//{
//	//HAL_GPIO_TogglePin(LED_03_GPIO_Port, LED_03_Pin);
//	HAL_UART_IRQHandler(get_huart3());
//}

