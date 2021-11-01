/*
 * config.h
 *
 *  Created on: Mar 20, 2021
 *      Author: harald
 */

#ifndef INSTANCES_CONFIG_H_
#define INSTANCES_CONFIG_H_

#ifdef __x86_64
#include <stm32f1xx.h>
//#include <stm32f1xx_hal_def.h>

#else
#include "stm32f1xx_hal.h"
#include <main.h>

// UART for debugging output and commandline input
extern UART_HandleTypeDef huart1; // see main.c
#define SERIAL_UART huart1
#endif

// I2C for AT24Cxx EEPROM
extern I2C_HandleTypeDef hi2c1; // see main.c
#define EEPROM_HI2C hi2c1;
#define EEPROM_I2C_ADDRESS 0xA0;



#endif /* INSTANCES_CONFIG_H_ */
