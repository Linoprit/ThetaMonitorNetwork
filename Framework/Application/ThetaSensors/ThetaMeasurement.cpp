/*
 * ThetaMeasurement.cpp
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#include <Application/ThetaSensors/ThetaMeasurement.h>
#include <System/OsHelpers.h>
#include <Sockets/CrcSocket.h>
#include <new>

// remove in productive
#include <System/serialPrintf.h>
#include <Libraries/HelpersLib.h>
#include <string>

namespace msmnt {
using namespace oneWire;
using namespace boschBme280;

void ThetaMeasurement::init(void) {
	new (&instance()) ThetaMeasurement();

	owCh1 = OneWire(OW_CH1_TX_PORT, OW_CH1_TX_PIN, OW_CH1_RX_PORT,
	OW_CH1_RX_PIN);
	owCh2 = OneWire(OW_CH2_TX_PORT, OW_CH2_TX_PIN, OW_CH2_RX_PORT,
	OW_CH2_RX_PIN);
	ds18B20Ch1 = DS18B20(&owCh1);
	ds18B20Ch2 = DS18B20(&owCh2);

	_bme280IdTheta = makeBmeId(SensorIdTable::SensorType::TEMP);
	_bme280IdHumid = makeBmeId(SensorIdTable::HUMIDITY);
	_bme280IdPress = makeBmeId(SensorIdTable::PRESS);
}

uint32_t ThetaMeasurement::makeBmeId(SensorIdTable::SensorType sensorType) {
	uint32_t plainId[2];
	// we need to swap the bytes
	plainId[0] = __builtin_bswap32(_nonVolatileData.getStatId());
	plainId[1] = __builtin_bswap32(static_cast<uint32_t>(sensorType));
	return CrcSocket::calc_chksum32(plainId, 2);
}

ThetaMeasurement& ThetaMeasurement::instance(void) {
	static ThetaMeasurement thetaMeasurement;
	return thetaMeasurement;
}

ThetaMeasurement::ThetaMeasurement() :
		_lastUpdate { 0 }, _bme280IdTheta { 0 }, _bme280IdHumid { 0 }, _bme280IdPress {
				0 } {
	for (SensorMeasureType &sensorType : sensorMeasureTable) {
		sensorType.sensorIdHash = NonVolatileData::EMPTY_SENSOR_HASH;
	}
}

void ThetaMeasurement::initSensors(void) {
	initTwoChannelDS1820();
	initBme280();

	cycle(); // fill the sensorTable array with valid data
	OsHelpers::delay(10);

	std::array<uint32_t, MAX_SENSORS> allSensorIds;
	uint8_t count = 0;
	for (const SensorMeasureType &sensorType : sensorMeasureTable) {
		uint32_t hashId = sensorType.sensorIdHash;
		if (hashId != 0) {
			allSensorIds.at(count) = hashId;
		} else {
			allSensorIds.at(count) = UINT32_MAX; // invalid hash
		}
		count++;
	}
	_sensorIdTable.initTable(&_nonVolatileData, allSensorIds);
}

void ThetaMeasurement::initBme280(void) {
	bme280.init(&BME280_HI2C, TEMPERATURE_16BIT, PRESSURE_ULTRALOWPOWER,
	HUMINIDITY_STANDARD, NORMALMODE);
	bme280.setConfig( STANDBY_MS_10, FILTER_OFF);
}

uint8_t ThetaMeasurement::cycleBme280(uint8_t index) {
	uint8_t lastIndex = index;

	float temperature;
	float huminidity;
	int32_t pressure;

	bme280.readTemperaturePressureHuminidity(&temperature, &pressure,
			&huminidity);

	sensorMeasureTable.at(lastIndex).sensorIdHash = _bme280IdTheta;
	sensorMeasureTable.at(lastIndex).value = temperature;
	sensorMeasureTable.at(lastIndex).sensorType = SensorIdTable::TEMP;
	lastIndex++;

	sensorMeasureTable.at(lastIndex).sensorIdHash = _bme280IdHumid;
	sensorMeasureTable.at(lastIndex).value = huminidity;
	sensorMeasureTable.at(lastIndex).sensorType = SensorIdTable::HUMIDITY;
	lastIndex++;

	sensorMeasureTable.at(lastIndex).sensorIdHash = _bme280IdPress;
	sensorMeasureTable.at(lastIndex).value = static_cast<float>(pressure)
			/ 100.0f; // [hPa]
	sensorMeasureTable.at(lastIndex).sensorType = SensorIdTable::PRESS;
	lastIndex++;

	return lastIndex;
}

void ThetaMeasurement::printDS1820Channel(DS18B20 ds18Channel) {
	DS18B20::DS1820SensorType *sensors = ds18Channel.getAllSensors();
	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		// we do not check the valid-flat
		float theta = sensors[i].temperature;
		std::string thetaStr = HelpersLib::floatToStr(theta, 2u);
		tx_printf(thetaStr.c_str());
		tx_printf("°C ");
	}
}

void ThetaMeasurement::initTwoChannelDS1820(void) {
	owCh1.initTimer(); // Init Timer 1
	//owCh2.init(); uses the same timer => notNeeded

	int8_t resultCh1 = ds18B20Ch1.findAllSensors();
	tx_printf("Sensors channel 1: %i\n", resultCh1);
	uint8_t resultCh2 = ds18B20Ch2.findAllSensors();
	tx_printf("Sensors channel 2: %i\n", resultCh2);

	ds18B20Ch1.setAllResolution(DS18B20::Resolution_12bits);
	ds18B20Ch2.setAllResolution(DS18B20::Resolution_12bits);
}

uint8_t ThetaMeasurement::storeToSensorTable(DS18B20 ds18Channel,
		uint8_t index) {
	uint8_t lastIndex = index;

	DS18B20::DS1820SensorType *sensors = ds18Channel.getAllSensors();
	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		if (sensors[i].dataIsValid) {
			sensorMeasureTable.at(lastIndex).sensorIdHash =
					SensorIdTable::sensorID2Hash(sensors[i].address);
			sensorMeasureTable.at(lastIndex).value = sensors[i].temperature;
			sensorMeasureTable.at(lastIndex).sensorType = SensorIdTable::TEMP;
			lastIndex++;
		}
	}
	return lastIndex;
}

uint8_t ThetaMeasurement::cycleTwoChannelsDS1820(uint8_t index) {
	uint8_t lastIndex = index;

	if (ds18B20Ch1.doAllMeasure() == true) {
		lastIndex = storeToSensorTable(ds18B20Ch1, lastIndex);
	}
	if (ds18B20Ch2.doAllMeasure() == true) {
		lastIndex = storeToSensorTable(ds18B20Ch2, lastIndex);
	}
	return lastIndex;
}

void ThetaMeasurement::cycle(void) {
	uint8_t lastIndex = 0;

	lastIndex = cycleTwoChannelsDS1820(lastIndex);
	lastIndex = cycleBme280(lastIndex);
	_lastUpdate = OsHelpers::get_tick() / 1000; // ms to s

	// TODO remove after testing
	for (uint8_t i = 0; i < lastIndex; i++) {
		SensorMeasureType actSensor = sensorMeasureTable.at(i);
		std::string valueStr = HelpersLib::floatToStr(actSensor.value, 2u);
		std::string sensorTypeStr = SensorIdTable::sensorType2Str(
				actSensor.sensorType);

		SensorIdTable::SensorConfigType sensorConfig =
				_sensorIdTable.getSensorData(actSensor.sensorIdHash);
		std::string shortname = std::string(sensorConfig.shortname, 8);

		tx_printf("%s / %lu: %s, %s\n", shortname.c_str(),
				actSensor.sensorIdHash, valueStr.c_str(),
				sensorTypeStr.c_str());
	}



}

} // namespace msmnt
