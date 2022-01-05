/*
 * ThetaMeasurement.h
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_
#define FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_

#include <Application/ThetaSensors/NonVolatileData.h>
#include <Application/ThetaSensors/SensorIdTable.h>
#include <Config/config.h>
#include <Devices/BME280/BME280.h>
#include <Devices/ds18b20/DS18B20.h>
#include <Devices/ds18b20/OneWire.h>
#include <sys/_stdint.h>
#include <array>

// RTOS definition done in main.c
extern osSemaphoreId_t measureArraySemHandle;

#define	 PACKED	__attribute__ ((packed))

namespace msmnt {

class ThetaMeasurement {
public:
	// must be packed, so we can put it into a RadioMessageType
	typedef struct PACKED {
		uint32_t sensorIdHash;
		float value;
		uint32_t lastUpdateTick;
	} MeasurementType;
	typedef std::array<MeasurementType, MAX_MEASUREMENTS> MeasurementArray;

	void init(void);
	static ThetaMeasurement& instance(void);
	void initHardware(void);
	bool isInitDone(void) {
		return _initIsDone;
	}
	void cycle(void);
	void updateMeasurementArray(uint32_t sensorIdHash, float value);
	uint8_t getValidMeasurementCount(void);

	uint8_t getFoundSensors(void) {
		return _foundDS1820Count;
	}
	MeasurementArray* getsensorMeasureTable(void) {
		return &measurementArray;
	}
	SensorIdTable* getSensorIdTable(void) {
		return &_sensorIdTable;
	}
	uint32_t getLastUpdateTick(void) {
		return _lastUpdateTick;
	}
	NonVolatileData* getNonVolatileData(void) {
		return &_nonVolatileData;
	}

private:
	ThetaMeasurement();
	virtual ~ThetaMeasurement() {
	}

	uint8_t fillSensorIdTable(oneWire::DS18B20 ds18Channel);
	void initBme280(void);
	void cycleBme280(void);

	void printDS1820Channel(oneWire::DS18B20 ds18Channel);
	void initTwoChannelDS1820(void);
	void cycleTwoChannelsDS1820(void);
	void storeDS1820ToMeasureArray(oneWire::DS18B20 ds18Channel);
	uint32_t makeBmeId(SensorIdTable::SensorType sensorType);

private:
	uint32_t _lastUpdateTick; // [s] last measurement update, derived from sys_tick
	uint32_t _bme280IdTheta;
	uint32_t _bme280IdHumid;
	uint32_t _bme280IdPress;
	bool _initIsDone;
	uint8_t _foundDS1820Count;

	NonVolatileData _nonVolatileData;
	SensorIdTable _sensorIdTable;

	oneWire::OneWire owCh1;
	oneWire::OneWire owCh2;
	oneWire::DS18B20 ds18B20Ch1;
	oneWire::DS18B20 ds18B20Ch2;
	boschBme280::Bme280 bme280;

	// measurement of the sensors, directly connected to this station,
	// and sent by the nRF-Module.
	// drawback: for the connected sensors, the same data is burried in
	// DS1820SensorType-array. Could be optimized...
	MeasurementArray measurementArray;
};

} // namespace msmnt

#endif /* FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_ */
