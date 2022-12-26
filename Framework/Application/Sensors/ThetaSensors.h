/*
 * ThetaMeasurement.h
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_
#define FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_

#include <Config/config.h>
#include <System/Devices/BME280/BME280.h>
#include <System/Devices/ds18b20/DS18B20.h>
#include <System/Devices/ds18b20/OneWire.h>
#include <Application/Sensors/SensorIdTable.h>
#include <Application/Sensors/Measurements.h>
#include <sys/_stdint.h>
#include <array>

#define	 PACKED	__attribute__ ((packed))

namespace snsrs {

class ThetaSensors {
public:
	ThetaSensors();
	virtual ~ThetaSensors() {
	}

	void init(uint32_t stationId);
	void initHardware(void);
	bool isInitDone(void) {
		return _initIsDone;
	}
	void cycle(void);

	uint8_t getFoundDS1820(void) {
		return _foundDS1820Count;
	}
	ThetaMsmnt::MeasurementArray* getMeasurementArray(void) {
		return _measurementArray.getArray();
	}
	ThetaMsmnt* getMeasurements(void) {
		return &_measurementArray;
	}
	uint32_t getLastUpdateTick(void) {
		return _lastUpdateTick;
	}
	void checkForTimeout(void);

private:
	// measurement of the sensors, directly connected to this station.	.
	// drawback: for the connected sensors, the same data is burried in
	// DS1820SensorType-array. Could be optimized...
	ThetaMsmnt _measurementArray;

	uint32_t _lastUpdateTick; // [s] last measurement update, derived from sys_tick

	bool _initIsDone;

	boschBme280::Bme280 bme280;
	uint32_t _bme280IdTheta;
	uint32_t _bme280IdHumid;
	uint32_t _bme280IdPress;

	oneWire::OneWire owCh1;
	oneWire::OneWire owCh2;
	oneWire::DS18B20 ds18B20Ch1;
	oneWire::DS18B20 ds18B20Ch2;
	uint8_t _foundDS1820Count;

	uint8_t fillSensorIdTable(oneWire::DS18B20 ds18Channel);
	void initBme280(void);
	void cycleBme280(void);

	void printDS1820Channel(oneWire::DS18B20 ds18Channel);
	void initTwoChannelDS1820(void);
	void cycleTwoChannelsDS1820(void);
	void storeDS1820ToMeasureArray(oneWire::DS18B20 ds18Channel);
	uint32_t makeBmeId(uint32_t stationId,
			SensorIdTable::SensorType sensorType);
};

} // namespace msmnt

#endif /* FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_ */
