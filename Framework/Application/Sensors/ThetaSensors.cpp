/*
 * ThetaMeasurement.cpp
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#include <System/OsHelpers.h>
#include <System/Sockets/CrcSocket.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/Sensors/NonVolatileData.h>
#include <new>
#include <Config/config.h>

// remove in productive
#include <System/serialPrintf.h>
#include <Libraries/HelpersLib.h>
#include <string>

namespace snsrs {
using namespace oneWire;
using namespace boschBme280;

ThetaSensors::ThetaSensors() :
		_measurementArray { localMsmntSemHandle }, _lastUpdateTick { 0 }, _initIsDone {
				false }, _bme280IdTheta { 0 }, _bme280IdHumid { 0 }, _bme280IdPress {
				0 }, _foundDS1820Count { 0 } {
}

void ThetaSensors::init(uint32_t stationId) {
	owCh1 = OneWire(OW_CH1_TX_PORT, OW_CH1_TX_PIN, OW_CH1_RX_PORT,
	OW_CH1_RX_PIN);
	owCh2 = OneWire(OW_CH2_TX_PORT, OW_CH2_TX_PIN, OW_CH2_RX_PORT,
	OW_CH2_RX_PIN);
	ds18B20Ch1 = DS18B20(&owCh1);
	ds18B20Ch2 = DS18B20(&owCh2);

	_bme280IdTheta = makeBmeId(stationId, SensorIdTable::SensorType::TEMP);
	_bme280IdHumid = makeBmeId(stationId, SensorIdTable::HUMIDITY);
	_bme280IdPress = makeBmeId(stationId, SensorIdTable::PRESS);
}

void ThetaSensors::initHardware(void) {
	initTwoChannelDS1820();
	initBme280();



	_initIsDone = true;
}

void ThetaSensors::initBme280(void) {
	bme280.init(&BME280_HI2C, TEMPERATURE_16BIT, PRESSURE_ULTRALOWPOWER,
	HUMINIDITY_STANDARD, NORMALMODE);
	bme280.setConfig(STANDBY_MS_10, FILTER_OFF);
}

void ThetaSensors::cycleBme280(void) {
	float temperature;
	float huminidity;
	int32_t pressure;

	bme280.readTemperaturePressureHuminidity(&temperature, &pressure,
			&huminidity);

	_measurementArray.update(_bme280IdTheta, temperature);
	_measurementArray.update(_bme280IdHumid, huminidity);
	_measurementArray.update(_bme280IdPress,
			static_cast<float>(pressure) / 100.0f); // [hPa]
}

uint32_t ThetaSensors::makeBmeId(uint32_t stationId,
		SensorIdTable::SensorType sensorType) {
	uint32_t plainId[2];
	// we need to swap the bytes
	plainId[0] = __builtin_bswap32(stationId);
	plainId[1] = __builtin_bswap32(static_cast<uint32_t>(sensorType));
	return CrcSocket::calcChksum32(plainId, 2);
}

void ThetaSensors::printDS1820Channel(DS18B20 ds18Channel) {
	DS18B20::DS1820SensorType *sensors = ds18Channel.getAllSensors();
	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		// we do not check the valid-flag
		float theta = sensors[i].temperature;
		std::string thetaStr = HelpersLib::floatToStr(theta, 2u);
		tx_printf(thetaStr.c_str());
		tx_printf("°C ");
	}
}

void ThetaSensors::initTwoChannelDS1820(void) {
	owCh1.initTimer(); // Init Timer 1
	//owCh2.init(); uses the same timer => notNeeded

	int8_t resultCh1 = ds18B20Ch1.findAllSensors();
	_foundDS1820Count = resultCh1;
	tx_printf("Sensors channel 1: %i\n", resultCh1);

	uint8_t resultCh2 = ds18B20Ch2.findAllSensors();
	tx_printf("Sensors channel 2: %i\n", resultCh2);
	_foundDS1820Count += resultCh2;

	ds18B20Ch1.setAllResolution(DS18B20::Resolution_12bits);
	ds18B20Ch2.setAllResolution(DS18B20::Resolution_12bits);
}

void ThetaSensors::storeDS1820ToMeasureArray(DS18B20 ds18Channel) {
	DS18B20::DS1820SensorType *sensors = ds18Channel.getAllSensors();

	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		if (sensors[i].dataIsValid) {
			_measurementArray.update(
					SensorIdTable::sensorID2Hash(sensors[i].address),
					sensors[i].temperature);
		} else {
			_measurementArray.update(
					SensorIdTable::sensorID2Hash(sensors[i].address),
					NAN);
		}
	}
}

void ThetaSensors::cycleTwoChannelsDS1820(void) {
	if (ds18B20Ch1.doAllMeasure() == true) {
		storeDS1820ToMeasureArray(ds18B20Ch1);
	}
	if (ds18B20Ch2.doAllMeasure() == true) {
		storeDS1820ToMeasureArray(ds18B20Ch2);
	}
}

void ThetaSensors::cycle(void) {
	cycleTwoChannelsDS1820();
	cycleBme280();
}

} // namespace msmnt
