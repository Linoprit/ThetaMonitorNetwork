/*
 * ID_Table.cpp
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#include <Application/ThetaSensors/SensorIdTable.h>
#include <Application/ThetaSensors/NonVolatileData.h>
#include <Sockets/CrcSocket.h>
#include <stddef.h>
#include <System/serialPrintf.h>

namespace msmnt {

SensorIdTable::SensorIdTable() {
	// NOP
}

void SensorIdTable::initTable(NonVolatileData *nvData,
		std::array<uint32_t, MAX_SENSORS> allSensorIds) {
	uint8_t index = 0;
	for (const uint32_t &sensorIdHash : allSensorIds) {
		index = copyItemToTable(nvData, sensorIdHash, index);
	}
	// for (SensorConfigType sensorConfig : sensorConfigTable) {
	// 	std::string shortname = std::string(sensorConfig.shortname, 8);
	// 	tx_printf("%lu %s\n", sensorConfig.sensorIdHash, shortname.c_str());
	// }
}

uint8_t SensorIdTable::copyItemToTable(NonVolatileData *nvData,
		uint32_t sensorIdHash, uint8_t index) {
	uint8_t lastIndex = index;

	SensorIdTable::SensorConfigType sensorIdE2 = nvData->getIdTableData(
			sensorIdHash);
	sensorConfigTable.at(lastIndex) = sensorIdE2;
	lastIndex++;

	return lastIndex;
}

SensorIdTable::SensorConfigType SensorIdTable::getSensorData(uint32_t hashId) {
	SensorConfigType sensorConfigResult;
	sensorConfigResult.sensorIdHash = NonVolatileData::EMPTY_SENSOR_HASH;
	for (SensorConfigType sensorConfig : sensorConfigTable) {
		if(sensorConfig.sensorIdHash == NonVolatileData::EMPTY_SENSOR_HASH){
			break;
		}
		if (sensorConfig.sensorIdHash == hashId) {
			sensorConfigResult = sensorConfig;
			break;
		}
	}
	return sensorConfigResult;
}

std::string SensorIdTable::sensorType2Str(uint8_t sensorType) {
	return sensorType2Str(static_cast<SensorType>(sensorType));
}
std::string SensorIdTable::sensorType2Str(SensorType sensorType) {
	std::string sensTypeStr;
	if (sensorType == SensorIdTable::SensorType::TEMP) {
		sensTypeStr = "TEMP";
	} else if (sensorType == SensorIdTable::SensorType::HUMIDITY) {
		sensTypeStr = "HUMI";
	} else if (sensorType == SensorIdTable::SensorType::PRESS) {
		sensTypeStr = "PRES";
	}
	return sensTypeStr;
}

uint32_t SensorIdTable::sensorID2Hash(uint8_t *address) {
	return CrcSocket::calc_chksum32(reinterpret_cast<uint32_t*>(address), 2);
}

} // namespace msmnt
