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

// OneWire definitions
extern TIM_HandleTypeDef htim1;
constexpr TIM_HandleTypeDef* OW_TIMER = &htim1;
constexpr bool OW_LINE_INVERTED = true;

#define OW_CH2_RX_PIN  CH2_RX_Pin
#define OW_CH2_RX_PORT CH2_RX_GPIO_Port
#define OW_CH2_TX_PIN  CH2_TX_Pin
#define OW_CH2_TX_PORT CH2_TX_GPIO_Port

#define OW_CH1_RX_PIN  CH1_RX_Pin
#define OW_CH1_RX_PORT CH1_RX_GPIO_Port
#define OW_CH1_TX_PIN  CH1_TX_Pin
#define OW_CH1_TX_PORT CH1_TX_GPIO_Port

// DS18B20
constexpr uint8_t SEARCH_RETRIES = 6;
constexpr uint8_t MAX_DEVICES_DS18B20 = 10;
// [ms] doAllMeasure(): how long to wait until all devices finished conversion
constexpr uint32_t	CONVERSION_TIMEOUT_MS = 5000;
#endif

// I2C for AT24Cxx EEPROM
extern I2C_HandleTypeDef hi2c1; // see main.c
#define EEPROM_HI2C hi2c1;
#define EEPROM_I2C_ADDRESS 0xA0;



#endif /* INSTANCES_CONFIG_H_ */
