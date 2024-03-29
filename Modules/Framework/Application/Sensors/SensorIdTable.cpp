/*
 * ID_Table.cpp
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#include <Application/Sensors/NonVolatileData.h>
#include <Application/Sensors/SensorIdTable.h>
#include <System/Sockets/CrcSocket.h>
#include <stddef.h>
#include <System/serialPrintf.h>

namespace snsrs {

SensorIdTable::SensorIdTable() {
	SensorIdType sensorId;
	sensorId.sensorIdHash = NonVolatileData::EMPTY_SENSOR_HASH;
	sensorIdArray.fill(sensorId);
}

SensorIdTable::SensorIdType SensorIdTable::getSensorTableData(
		NonVolatileData *nvData, uint32_t hashId) {

	if (osSemaphoreAcquire(tableSensDataSemHandle, 10) != osOK) {
		SensorIdTable::SensorIdType empty;
		empty.sensorIdHash = NonVolatileData::EMPTY_SENSOR_HASH;
		return empty;
	}

	SensorIdType *sensorId = NULL;
	for (uint8_t i = 0; i < sensorIdArray.size(); i++) {
		sensorId = &sensorIdArray.at(i);
		if (sensorId->sensorIdHash == NonVolatileData::EMPTY_SENSOR_HASH) {
			SensorIdTable::SensorIdType sensorIdE2 = nvData->getIdTableData(
					hashId);
			*sensorId = sensorIdE2;
			break;
		} else if (sensorId->sensorIdHash == hashId) {
			break;
		}
	}

	osSemaphoreRelease(tableSensDataSemHandle);
	return *sensorId;
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

std::string SensorIdTable::getUnit(SensorType sensorType) {
	std::string sensTypeStr;
	if (sensorType == SensorIdTable::SensorType::TEMP) {
		sensTypeStr = "°C";
	} else if (sensorType == SensorIdTable::SensorType::HUMIDITY) {
		sensTypeStr = "%";
	} else if (sensorType == SensorIdTable::SensorType::PRESS) {
		sensTypeStr = "hPa";
	}
	return sensTypeStr;
}

uint32_t SensorIdTable::sensorID2Hash(uint8_t *address) {
	return CrcSocket::calcChksum32(reinterpret_cast<uint32_t*>(address), 2);
}

} // namespace msmnt
