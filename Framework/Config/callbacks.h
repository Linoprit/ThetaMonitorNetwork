/*
 * callbacks.h
 *
 *  Created on: 17.05.2016
 *      Author: harald
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_



// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
#ifdef __x86_64
#include "stm32f1xx_hal.h"
#elif defined STM32F401xE || defined STM32F401xC
#include "stm32f4xx_hal.h"
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#endif
#include <stdlib.h>

// C only
EXTERNC void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#undef EXTERNC
#ifdef __cplusplus

// cpp stuff here...

#endif // C interface

/*
 for plantUml we declare this file as a class
@startuml callbacks_man.png
class callbacks {
	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart)
	void USART1_IRQHandler(void)
}
@enduml
*/

#endif /* CALLBACKS_H_ */
