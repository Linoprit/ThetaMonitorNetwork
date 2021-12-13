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

// ******* UART for debugging output and commandline input *******
extern UART_HandleTypeDef huart1; // see main.c
#define SERIAL_UART huart1

// ******* OneWire and DS18B20 definitions *******
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

// ******* DS18B20 device *******
constexpr uint8_t DS18B20_SEARCH_RETRIES = 6;
constexpr uint8_t DS18B20_MAX_DEVICES = 6; // in one channel
// [ms] doAllMeasure(): how long to wait until all devices finished conversion
constexpr uint32_t	DS18B20_CONVERSION_TIMEOUT_MS = 5000;

// ******* I2C for AT24Cxx EEPROM *******
extern I2C_HandleTypeDef hi2c1; // see main.c
#define EEPROM_HI2C hi2c1
#define EEPROM_I2C_ADDRESS 0xA0

// ******* I2C for BME280 *******
extern I2C_HandleTypeDef hi2c2; // see main.c
#define BME280_HI2C hi2c2
//#define BME280_I2CADDR	0xEC		// 0x76<<1	SDO -> GND
#define BME280_I2CADDR	0xEE			// 0x77<<1	SDO -> VCC
extern TIM_HandleTypeDef htim2;
constexpr TIM_HandleTypeDef* BME280_TIMER = &htim2;

// ******* SPI for nRF24L01 *******
extern SPI_HandleTypeDef hspi1;
#define nRF24_SPI hspi1
// Pins must be named like this, to get them defined in main.h
// NRF_IRQ_GPIO_Port, NRF_IRQ_Pin
// NRF_CE_GPIO_Port, NRF_CE_Pin
// NRF_CSN_GPIO_Port,  NRF_CSN_Pin
// Enable IRQ on IRQ-Pin, i.E. EXTI0

// TODO must be altered
#define nRF_PAYLOAD_LEN	13 // nRF24 payload length for RX and TX


// ******* Common  *******
// DS1820 Sensors * Channels + BME280 Temp/Humi/Press
constexpr uint8_t MAX_SENSORS = DS18B20_MAX_DEVICES * 2 + 3;

#endif // __x86_64
#endif /* INSTANCES_CONFIG_H_ */
