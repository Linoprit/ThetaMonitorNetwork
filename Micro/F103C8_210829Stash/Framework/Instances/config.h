/*
 * config.h
 *
 *  Created on: Mar 20, 2021
 *      Author: harald
 */

#ifndef INSTANCES_CONFIG_H_
#define INSTANCES_CONFIG_H_

#include "stm32f1xx_hal.h"
#include <main.h>

// UART for debugging output and commandlin input
extern UART_HandleTypeDef huart1;
#define SERIAL_UART huart1





#endif /* INSTANCES_CONFIG_H_ */
