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
#include <stdlib.h>
#include "stm32f1xx_hal.h"


//EXTERNC void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#undef EXTERNC
#ifdef __cplusplus

#endif // C interface

#endif /* CALLBACKS_H_ */
