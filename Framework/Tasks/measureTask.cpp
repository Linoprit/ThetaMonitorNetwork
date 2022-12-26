
/*
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#elif defined STM32F401xE
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#endif
#include "tasksDef.h"
#include <Config/config.h>
#include <Application/Sensors/Sensors.h>
#include <System/OsHelpers.h>

#ifdef __x86_64
void startMeasureTask(void *argument) {
	UNUSED(argument);
	for (;;) {
		OsHelpers::delay(1000);
	}
#else

void startMeasureTask(void *argument) {
	UNUSED(argument);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	snsrs::Sensors::instance().init();
	snsrs::Sensors::instance().initHardware();

	for (;;) {
		// TODO remove
		HAL_GPIO_TogglePin(DebLed_1_GPIO_Port, DebLed_1_Pin);

		snsrs::Sensors::instance().cycle();
		OsHelpers::delay(MEASURETASK_CYCLE);
	}
}

// just to keep it, if I need it later...

// in the task
// initTwoChannelTestOwnOw();
// initBme280Test();
// OsHelpers::delay(3000);
//
// for (;;) {
//
// 	cycleTwoChannelTestOwnOw();
// 	OsHelpers::delay(1000);
//
// 	cycleBme280Test();
// 	OsHelpers::delay(1000);
// }

// #include <Devices/ds18b20/OneWire.h>
// #include <Devices/ds18b20/DS18B20.h>
// #include <Devices/BME280/BME280.h>
// #include <Devices/BME280/delays.h>
//#include <System/serialPrintf.h>
//#include <Libraries/HelpersLib.h>
//#include <string>

// using namespace oneWire;
// oneWire::OneWire owCh1 = OneWire(OW_CH1_TX_PORT, OW_CH1_TX_PIN, OW_CH1_RX_PORT,
// OW_CH1_RX_PIN);
// oneWire::OneWire owCh2 = OneWire(OW_CH2_TX_PORT, OW_CH2_TX_PIN, OW_CH2_RX_PORT,
// OW_CH2_RX_PIN);
// DS18B20 ds18B20Ch1 = DS18B20(&owCh1);
// DS18B20 ds18B20Ch2 = DS18B20(&owCh2);
//
// void printChannel(DS18B20 ds18Channel) {
// 	DS18B20::SensorType *sensors = ds18Channel.getAllSensors();
// 	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
// 		// we do not check the valid-flat
// 		float theta = sensors[i].temperature;
// 		std::string thetaStr = HelpersLib::floatToStr(theta, 2u);
// 		tx_printf(thetaStr.c_str());
// 		tx_printf("°C ");
// 	}
// }
//
// void initTwoChannelTestOwnOw(void) {
// 	owCh1.initTimer(); // Init Timer 1
// 	//owCh2.init(); uses the same timer => notNeeded
//
// 	int8_t resultCh1 = ds18B20Ch1.findAllSensors();
// 	tx_printf("Sensors channel 1: %i\n", resultCh1);
// 	uint8_t resultCh2 = ds18B20Ch2.findAllSensors();
// 	tx_printf("Sensors channel 2: %i\n", resultCh2);
//
// 	ds18B20Ch1.setAllResolution(DS18B20::Resolution_12bits);
// 	ds18B20Ch2.setAllResolution(DS18B20::Resolution_12bits);
// }
//
// void cycleTwoChannelTestOwnOw(void) {
//
// 	if (ds18B20Ch1.doAllMeasure() == true) {
// 		tx_printf("Channel 1: ");
// 		printChannel(ds18B20Ch1);
// 		tx_printf("\n");
//
// 		HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_SET);
// 	} else {
// 		HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_RESET);
// 	}
//
// 	if (ds18B20Ch2.doAllMeasure() == true) {
// 		tx_printf("Channel 2: ");
// 		printChannel(ds18B20Ch2);
// 		tx_printf("\n");
//
// 		HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_SET);
// 	} else {
// 		HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_RESET);
// 	}
//
// 	tx_printf("\n");
// }
//
// using namespace boschBme280;
// Bme280 bme280;
//
// void initBme280Test(void) {
// 	bme280.init(&BME280_HI2C, TEMPERATURE_16BIT, PRESSURE_ULTRALOWPOWER,
// 	HUMINIDITY_STANDARD, NORMALMODE);
// 	bme280.setConfig( STANDBY_MS_10, FILTER_OFF);
// }
//
// void cycleBme280Test(void) {
//
// 	float temperature;
// 	float huminidity;
// 	int32_t pressure;
//
// 	bme280.readTemperaturePressureHuminidity(&temperature, &pressure,
// 			&huminidity);
//
// 	std::string temperatureStr = HelpersLib::floatToStr(temperature, 2u);
// 	tx_printf("Theta: ");
// 	tx_printf(temperatureStr.c_str());
// 	tx_printf(" °C / ");
//
// 	// Taupunkt
// 	float dewpointTemperature = (237.7
// 			* (((17.27 * temperature) / (237.7 + temperature))
// 					+ logf(huminidity / 100)))
// 			/ (17.27
// 					- (((17.27 * temperature) / (237.7 + temperature))
// 							+ logf(huminidity / 100)));
//
// 	std::string dewpointTemperatureStr = HelpersLib::floatToStr(
// 			dewpointTemperature, 2u);
// 	tx_printf("dew: ");
// 	tx_printf(dewpointTemperatureStr.c_str());
// 	tx_printf(" °C / ");
//
// 	// 1 hPa = 1 mbar
// 	std::string pressureStr = HelpersLib::floatToStr((pressure / 100.0f), 2u);
// 	tx_printf("press: ");
// 	tx_printf(pressureStr.c_str());
// 	tx_printf(" hPa / ");
//
// 	std::string huminidityStr = HelpersLib::floatToStr(huminidity, 2u);
// 	tx_printf("hum: ");
// 	tx_printf(huminidityStr.c_str());
// 	tx_printf(" %% ");
//
// 	float seaLevelPa = 1013.25f;
// 	float altitude = bme280.readAltitude(seaLevelPa);
// 	std::string altitudeStr = HelpersLib::floatToStr(altitude, 2u);
// 	tx_printf("alt: ");
// 	tx_printf(altitudeStr.c_str());
// 	tx_printf(" m ");
//
// 	tx_printf("\n");
// }

#endif // ifdef __x86_64
