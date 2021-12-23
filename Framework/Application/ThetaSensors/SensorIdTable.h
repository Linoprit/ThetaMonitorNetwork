/*
 * ID_Table.h
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#ifndef THETASENSORS_ID_TABLE_H_
#define THETASENSORS_ID_TABLE_H_

#include <Config/config.h>
//#include <Application/ThetaSensors/NonVolatileData.h>
#include <stdint.h>
#include <string>
#include <array>

#ifdef __x86_64
#include <iostream>
#include <boost/format.hpp>
using namespace std;
#endif

/**
 * Station IDs:
 * 0 = invalid
 * 0x01 - 0xFF = MasterStation
 * 0x100 - 0x1FF = SlaveStation
 * nRF-Pipes of slaves:
 * 0x101 = Slave, Pipe1
 * 0x102 = Slave, Pipe2
 * 0x103 = Slave, Pipe3
 * ...
 */

namespace msmnt {

class NonVolatileData;

class SensorIdTable {
public:
	static constexpr uint16_t SENSOR_TABLE_LEN = 22;
	static constexpr uint16_t SHORTNAME_LEN = 8;
	static constexpr uint16_t INVLD_TEMPERATURE = 255;
	static constexpr uint16_t MIN_TEMP_HYSTERESIS = 1;
	static constexpr uint16_t SENSOR_ID_LEN = 8;

	enum SensorType {
		TEMP = 0, HUMIDITY = 1, PRESS = 2
	};
	enum StationType {
		NONE = 0, MASTER, SLAVE_01, SLAVE_02, SLAVE_03, SLAVE_04, SLAVE_05
	};

	typedef struct {
		uint32_t sensorIdHash;
		float minVal;
		float maxVal;
		SensorType sensType;
		uint8_t relayNr; // 0 = no relay
		char shortname[SensorIdTable::SHORTNAME_LEN];
	} SensorIdType; // to be used in application
	typedef std::array<SensorIdType, MAX_MEASUREMENTS> SensorIdArray;

	SensorIdTable();
	virtual ~SensorIdTable() {
	}

	SensorIdTable::SensorIdType getSensorId(NonVolatileData *nvData,
			uint32_t hashId);

	static std::string sensorType2Str(SensorType sensorType);
	static std::string sensorType2Str(uint8_t sensorType);
	static std::string getUnit(SensorType sensorType);
	// Takes in the DS1820 address-pattern and returns it hash-value
	static uint32_t sensorID2Hash(uint8_t *address); // len = 8!

#ifdef __x86_64

	static void dump (SensorIdType sens){
		cout << to_string(sens.sensorIdHash) << " ";
		cout << boost::format("%3.2f") % sens.minVal << " ";
		cout << boost::format("%3.2f") % sens.maxVal << " ";
		cout << sensorType2Str(sens.sensType) << " ";
		cout << to_string(sens.relayNr) << " ";
		string name(sens.shortname, SHORTNAME_LEN);
		cout << name << endl;
	}

#endif

private:
	// information about the sensors, ram-mirror of what is in E2
	SensorIdArray sensorIdArray;
};

} // namespace msmnt

#endif /* THETASENSORS_ID_TABLE_H_ */
