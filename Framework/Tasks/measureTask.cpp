/*
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#else
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#include <Config/config.h>
#include <Devices/ds18b20/OneWire.h>
#include <Devices/ds18b20/DS18B20.h>
#endif

#include <System/OsHelpers.h>
#include "tasksDef.h"
#include <System/serialPrintf.h>
#include <Libraries/HelpersLib.h>
#include <string>

#ifdef __x86_64
void startMeasureTask(void *argument) {
	UNUSED(argument);
	for (;;) {
		OsHelpers::delay(1000);
	}
#else

using namespace oneWire;
oneWire::OneWire owCh1 = OneWire(OW_CH1_TX_PORT, OW_CH1_TX_PIN, OW_CH1_RX_PORT,
OW_CH1_RX_PIN);
oneWire::OneWire owCh2 = OneWire(OW_CH2_TX_PORT, OW_CH2_TX_PIN, OW_CH2_RX_PORT,
OW_CH2_RX_PIN);
DS18B20 ds18B20Ch1 = DS18B20(&owCh1);
DS18B20 ds18B20Ch2 = DS18B20(&owCh2);

void printChannel(DS18B20 ds18Channel) {
	DS18B20::SensorType *sensors = ds18Channel.getAllSensors();
	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		// we do not check the valid-flat
		float theta = sensors[i].Temperature;
		std::string thetaStr = HelpersLib::floatToStr(theta, 2u);
		tx_printf(thetaStr.c_str());
		tx_printf("°C ");
	}
}

void callTwoChannelTestOwnOw(void) {
	owCh1.init();
//owCh2.init();

//uint8_t rstRes = owCh1.reset();
//tx_printf("rstRes %i\n", rstRes);
//owCh2.reset();
//OsHelpers::delay(500);

	uint8_t resultCh1 = ds18B20Ch1.findAllSensors();
	tx_printf("Sensors channel 1: %i\n", resultCh1);
	uint8_t resultCh2 = ds18B20Ch2.findAllSensors();
	tx_printf("Sensors channel 2: %i\n", resultCh2);

	ds18B20Ch1.setAllResolution(DS18B20::Resolution_12bits);
	ds18B20Ch2.setAllResolution(DS18B20::Resolution_12bits);

//volatile uint8_t resCh1 = ds18B20Ch1.getResolution(
//		ds18B20Ch1.getAllSensors()[0].Address);

	for (;;) {
		if (ds18B20Ch1.doAllMeasure() == true) {
			tx_printf("Channel 1: ");
			printChannel(ds18B20Ch1);
			tx_printf("\n");

			HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_RESET);
		}

		if (ds18B20Ch2.doAllMeasure() == true) {
			tx_printf("Channel 2: ");
			printChannel(ds18B20Ch2);
			tx_printf("\n");

			HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_RESET);
		}

		tx_printf("\n");
		osDelay(1000);
	}
}

void startMeasureTask(void *argument) {
	UNUSED(argument);

	for (;;) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		callTwoChannelTestOwnOw();

		OsHelpers::delay(1000);
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	}
#endif // ifdef __x86_64
}
