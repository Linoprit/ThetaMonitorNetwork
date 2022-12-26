/*
 * callbacks.c
 *
 *  Created on: 16.05.2016
 *      Author: harald
 */

#ifndef __x86_64
// FIXME make it working on X86

#include "callbacks.h"
//#include <System/uart_printf.h>
#include <Config/config.h>
#include <System/CommandLine/CommandLine.h>
#include <Application/Radio/RadioSlave.h>
#include <Application/Sensors/ThetaSensors.h>
#include <System/Sockets/GPIOSocket_nRF24.h>
#include <Application/Nokia_LCD/LCDFunctions.h>

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	//UNUSED(GPIO_Pin);

	if (GPIO_Pin == GPIOSocket_nRF24::get_IRQ_Pin()) {
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		// Hm. Since we call a base-class function... does it make sense to go
		// through the actual initialized class?
		bool isMaster =
				snsrs::Sensors::instance().getNonVolatileData()->getStationType()
						== snsrs::SensorIdTable::MASTER;
		if (isMaster) {
			radio::RadioMaster::instance().getNRF24L01_Basis()->IrqPinRxCallback();
		} else {
			radio::RadioSlave::instance().getNRF24L01_Basis()->IrqPinRxCallback();
		}

	} else if (GPIO_Pin == BUTTON_1_Pin) {
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		lcd::LCDFunctions::instance().buttonPinCallback();
	}
}

uint8_t uart1Rx = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		// TODO put this into SerialPrint
		if (uart1Rx != 0) {
			cLine::CommandLine::instance().putChar(uart1Rx);
		}
		HAL_UART_Receive_IT(&SERIAL_UART, &uart1Rx, 1);
	} else if (huart->Instance == USART2) {
		// NOP no receptions on RASPY_UART
	}
	//else if (huart->Instance == USART3) {
		// NOP
	//}
}

// needed for dma-receive (extra stm32f1xx_hal_uart.* needed
void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart) {
	// call_uart_callbacks(huart);
	return;
}

// Workaround hanging usart-dma
// todo look, if this can be removed
void USART1_IRQHandler(void) {
	//HAL_GPIO_TogglePin(LED_03_GPIO_Port, LED_03_Pin);
	HAL_UART_IRQHandler(&huart1);
}
// void USART2_IRQHandler(void)
// {
//   HAL_UART_IRQHandler(get_huart2);
// }
// void USART3_IRQHandler(void)
// {
// 	//HAL_GPIO_TogglePin(LED_03_GPIO_Port, LED_03_Pin);
// 	HAL_UART_IRQHandler(get_huart3());
// }

// ownSysTick-Handler for sysTick counter in one second steps.
// defined under mx / rtos / Timers and Semaphores
void ownSysTick(void *argument)
{
	OsHelpers::inc_tick_seconds();
}

#endif
