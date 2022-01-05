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
	return CrcSocket::calcChksum32(plainId, 2);
}

ThetaMeasurement& ThetaMeasurement::instance(void) {
	static ThetaMeasurement thetaMeasurement;
	return thetaMeasurement;
}

ThetaMeasurement::ThetaMeasurement() :
		_lastUpdateTick { 0 }, _bme280IdTheta { 0 }, _bme280IdHumid { 0 }, _bme280IdPress {
				0 }, _initIsDone { false }, _foundDS1820Count { 0 } {
	for (MeasurementType &sensorType : measurementArray) {
		sensorType.sensorIdHash = NonVolatileData::EMPTY_SENSOR_HASH;
		sensorType.value = NAN;
		sensorType.lastUpdateTick = OsHelpers::get_tick();
	}
}

void ThetaMeasurement::initHardware(void) {
	initTwoChannelDS1820();
	initBme280();

	_initIsDone = true;
}

void ThetaMeasurement::initBme280(void) {
	bme280.init(&BME280_HI2C, TEMPERATURE_16BIT, PRESSURE_ULTRALOWPOWER,
	HUMINIDITY_STANDARD, NORMALMODE);
	bme280.setConfig( STANDBY_MS_10, FILTER_OFF);
}

void ThetaMeasurement::cycleBme280(void) {
	float temperature;
	float huminidity;
	int32_t pressure;

	bme280.readTemperaturePressureHuminidity(&temperature, &pressure,
			&huminidity);

	updateMeasurementArray(_bme280IdTheta, temperature);
	updateMeasurementArray(_bme280IdHumid, huminidity);
	updateMeasurementArray(_bme280IdPress,
			static_cast<float>(pressure) / 100.0f); // [hPa]
}

void ThetaMeasurement::printDS1820Channel(DS18B20 ds18Channel) {
	DS18B20::DS1820SensorType *sensors = ds18Channel.getAllSensors();
	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		// we do not check the valid-flag
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
	_foundDS1820Count = resultCh1;
	tx_printf("Sensors channel 1: %i\n", resultCh1);

	uint8_t resultCh2 = ds18B20Ch2.findAllSensors();
	tx_printf("Sensors channel 2: %i\n", resultCh2);
	_foundDS1820Count += resultCh2;

	ds18B20Ch1.setAllResolution(DS18B20::Resolution_12bits);
	ds18B20Ch2.setAllResolution(DS18B20::Resolution_12bits);
}

void ThetaMeasurement::storeDS1820ToMeasureArray(DS18B20 ds18Channel) {

	DS18B20::DS1820SensorType *sensors = ds18Channel.getAllSensors();

	for (uint8_t i = 0; i < ds18Channel.getFoundSensors(); i++) {
		if (sensors[i].dataIsValid) {
			updateMeasurementArray(
					SensorIdTable::sensorID2Hash(sensors[i].address),
					sensors[i].temperature);
		}
	}
}

void ThetaMeasurement::updateMeasurementArray(uint32_t sensorIdHash,
		float value) {

	//osStatus_t osStatus =
	osSemaphoreAcquire(measureArraySemHandle, 0);
	for (uint8_t i = 0; i < measurementArray.size(); i++) {
		MeasurementType *storedMeasurement = &measurementArray.at(i);
		if ((storedMeasurement->sensorIdHash == sensorIdHash)
				|| (storedMeasurement->sensorIdHash
						== NonVolatileData::EMPTY_SENSOR_HASH)) {
			storedMeasurement->sensorIdHash = sensorIdHash;
			storedMeasurement->value = value;
			storedMeasurement->lastUpdateTick = OsHelpers::get_tick();
			return;
		}
	}
	//osStatus =
	osSemaphoreRelease(measureArraySemHandle);

	// for (MeasurementType storedMeasurement : measurementArray) {
	// 	tx_printf("%ul\n", storedMeasurement.sensorIdHash);
	// }
}

void ThetaMeasurement::cycleTwoChannelsDS1820(void) {

	if (ds18B20Ch1.doAllMeasure() == true) {
		storeDS1820ToMeasureArray(ds18B20Ch1);
	}
	if (ds18B20Ch2.doAllMeasure() == true) {
		storeDS1820ToMeasureArray(ds18B20Ch2);
	}
}

void ThetaMeasurement::cycle(void) {
	cycleTwoChannelsDS1820();
	cycleBme280();

	// TODO remove after testing
	//	 for (uint8_t i = 0; i < MAX_SENSORS; i++) {
	//	 	MeasurementType actSensor = measurementArray.at(i);
	//	 	std::string valueStr = HelpersLib::floatToStr(actSensor.value, 2u);
	//
	//	 	SensorIdTable::SensorIdType sensorConfig = _sensorIdTable.getSensorId(
	//	 			&_nonVolatileData, actSensor.sensorIdHash);
	//	 	std::string shortname = std::string(sensorConfig.shortname, 8);
	//	 	std::string sensorTypeStr = SensorIdTable::sensorType2Str(
	//	 			sensorConfig.sensType);
	//
	//	 	tx_printf("%s / %lu: %s '%s'\n", shortname.c_str(),
	//	 			actSensor.sensorIdHash, valueStr.c_str(),
	//	 			sensorTypeStr.c_str());
	//	 }
	//	 tx_printf("\n");
}

uint8_t ThetaMeasurement::getValidMeasurementCount(void) {
	for (uint8_t i = 0; i < measurementArray.size(); i++) {
		if (measurementArray.at(i).sensorIdHash
				== NonVolatileData::EMPTY_SENSOR_HASH) {
			return i;
		}
	}
	return 0;
}

} // namespace msmnt
