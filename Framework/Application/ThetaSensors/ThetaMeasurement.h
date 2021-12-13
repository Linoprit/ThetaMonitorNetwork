/*
 * ThetaMeasurement.h
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_
#define FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_

#include <Application/ThetaSensors/NonVolatileData.h>
#include <Config/config.h>
#include <array>
#include <Application/ThetaSensors/SensorIdTable.h>
#include <Config/config.h>
#include <Devices/ds18b20/OneWire.h>
#include <Devices/ds18b20/DS18B20.h>
#include <Devices/BME280/BME280.h>

namespace msmnt {

class ThetaMeasurement {
public:
	typedef struct {
		uint32_t sensorIdHash;
		SensorIdTable::SensorType sensorType;
		float value;
	} SensorMeasureType;

	void init(void);
	static ThetaMeasurement& instance(void);
	void initSensors(void);
	void cycle(void);
	uint32_t getLastUpdate(void) {
		return _lastUpdate;
	}
	NonVolatileData getNonVolatileData(void) {
		return _nonVolatileData;
	}

private:
	ThetaMeasurement();
	virtual ~ThetaMeasurement() {
	}
	;
	uint32_t _lastUpdate; // [s] last measurement update, derived from sys_tick
	uint32_t _bme280IdTheta;
	uint32_t _bme280IdHumid;
	uint32_t _bme280IdPress;

	NonVolatileData _nonVolatileData;
	SensorIdTable _sensorIdTable;

	oneWire::OneWire owCh1;
	oneWire::OneWire owCh2;
	oneWire::DS18B20 ds18B20Ch1;
	oneWire::DS18B20 ds18B20Ch2;
	boschBme280::Bme280 bme280;

	// current measurements
	std::array<SensorMeasureType, MAX_SENSORS> sensorMeasureTable;

	uint8_t fillSensorIdTable(oneWire::DS18B20 ds18Channel, uint8_t index);
	uint8_t copyItemToTable(uint32_t sensorIdHash, uint8_t index);
	void initBme280(void);
	uint8_t cycleBme280(uint8_t index);

	void printDS1820Channel(oneWire::DS18B20 ds18Channel);
	void initTwoChannelDS1820(void);
	uint8_t cycleTwoChannelsDS1820(uint8_t index);

	uint8_t storeToSensorTable(oneWire::DS18B20 ds18Channel, uint8_t index);
	uint32_t makeBmeId(SensorIdTable::SensorType sensorType);
};

} // namespace msmnt

#endif /* FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_ */
