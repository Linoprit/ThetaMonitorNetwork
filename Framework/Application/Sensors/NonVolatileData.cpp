/*
 * NonVolatileData.cpp
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */

#include <Application/Sensors/NonVolatileData.h>
#include <System/Devices/Eeprom_at24c256/AT24Cxxx.h>
#include <System/Sockets/CrcSocket.h>
#include <Libraries/HelpersLib.h>
#include <Libraries/Convert.h>
#include <System/serialPrintf.h>
#include <cstring>
#include <System/OsHelpers.h>

namespace snsrs {

NonVolatileData::NonVolatileData() :
		_currAddress { 0 }, _oldAddress { 0 }, _statIdBuffered { 0 } {
	static_assert(sizeof(SensorTypeE2) == 16U);
}

// due to limitations on the embedded system we cannot print floats
void NonVolatileData::printIdTableRaw(void) {
	SensorTypeE2 idE2Data;

	Convert cTheta(convTheta);
	Convert cHumid(convHumid);
	Convert cPress(convPress);

	_currAddress = ID_TABLE_START;
	idE2Data = iter();
	if (dataIsEmpty(idE2Data)) {
		tx_printf("ID-Table is empty.\n");
	}
	while (!dataIsEmpty(idE2Data)) {
		uint8_t chkSum = calcChkSum(idE2Data);
		std::string shortname(idE2Data.shortname, SensorIdTable::SHORTNAME_LEN);
		shortname += "\0";
		std::string sensTypeStr = SensorIdTable::sensorType2Str(
				idE2Data.sensType);
		std::string minValStr;
		std::string maxValStr;
		if (idE2Data.sensType == static_cast<uint8_t>(SensorIdTable::TEMP)) {
			minValStr = HelpersLib::floatToStr(cTheta.int2Phys(idE2Data.minVal),
					1u);
			maxValStr = HelpersLib::floatToStr(cTheta.int2Phys(idE2Data.maxVal),
					1u);
		} else if (idE2Data.sensType
				== static_cast<uint8_t>(SensorIdTable::HUMIDITY)) {
			minValStr = HelpersLib::floatToStr(cHumid.int2Phys(idE2Data.minVal),
					1u);
			maxValStr = HelpersLib::floatToStr(cHumid.int2Phys(idE2Data.maxVal),
					1u);
		} else if (idE2Data.sensType
				== static_cast<uint8_t>(SensorIdTable::PRESS)) {
			minValStr = HelpersLib::floatToStr(cPress.int2Phys(idE2Data.minVal),
					1u);
			maxValStr = HelpersLib::floatToStr(cPress.int2Phys(idE2Data.maxVal),
					1u);
		}

		tx_printf("%u %s %s %s %i, %s, %i \n", idE2Data.sensorIdHash,
				minValStr.c_str(), maxValStr.c_str(), sensTypeStr.c_str(),
				idE2Data.relayNr, shortname.c_str(),
				(chkSum == idE2Data.checkSum));
		idE2Data = iter();

		tx_cycle(); // avoid tx-buffer overflow
		OsHelpers::delay(20);
	}
}

/*
 * Looks through the ID-Table section, if a given ID exists. If the hash wasn't found,
 * the sensorIdHash is set to zero in the returned struct.
 */
SensorIdTable::SensorIdType NonVolatileData::getIdTableData(
		uint32_t sensorIdHash) {
	SensorTypeE2 idE2Data;
	SensorIdTable::SensorIdType idPhysData;
	idPhysData.sensorIdHash = UINT32_MAX;

	_currAddress = ID_TABLE_START;
	idE2Data = iter();
	while (!dataIsEmpty(idE2Data)) {
		if ((idE2Data.checkSum == calcChkSum(idE2Data))
				&& (idE2Data.sensorIdHash == sensorIdHash)) {
			idPhysData = e2ToPhys(idE2Data);
			break;
		}
		idE2Data = iter();
	}
	return idPhysData;
}

/*
 * Writes EMPTY_SENS_ID (=UINT32_MAX) into the ID-Table section.
 * Thus 'formats' this section.
 */
ErrorCode NonVolatileData::clrIdTableData(void) {
	_currAddress = ID_TABLE_START;
	uint32_t emptySensId[4] = { EMPTY_SENSOR_HASH, EMPTY_SENSOR_HASH, EMPTY_SENSOR_HASH,
			EMPTY_SENSOR_HASH, };

	for (uint16_t i = 0; i < NUM_OF_ID_ENTRIES; i++) {
		AT24Cxxx::write(_currAddress, reinterpret_cast<uint8_t*>(&emptySensId),
				sizeof(SensorTypeE2));
		_currAddress += sizeof(SensorTypeE2);
	}
	return ERR_OK;
}

/*
 * Looks through the ID-Table section, if a given ID exists. The data is only renewed,
 * if it's different to idPhysData. If there is no matching ID, the data is written
 * to the first free address.
 */
ErrorCode NonVolatileData::writeIdTableData(
		SensorIdTable::SensorIdType idPhysData) {
	SensorTypeE2 idE2Data;
	SensorTypeE2 idE2ToWrite = physToE2(idPhysData);

	// sets _currAddress
	findSensIdHashOrEmpty(idE2ToWrite.sensorIdHash);
	AT24Cxxx::read(_currAddress, reinterpret_cast<uint8_t*>(&idE2Data),
			sizeof(SensorTypeE2));

	if (compareIdTableDatum(idE2Data, idE2ToWrite) == false) {
		AT24Cxxx::write(_currAddress, reinterpret_cast<uint8_t*>(&idE2ToWrite),
				sizeof(SensorTypeE2));
		return ERR_OK;
	}

	return ERR_OK_E2_NOT_WRITTEN;
}
/*
 * Returns the E2-address, if a match was found, otherwise the address of
 * the first empty slot.
 * The End of the valid data-block is recognized, by finding a UINT32_MAX.
 * So, initial the E2 must be erased by writing 0xFF to the whole device.
 */
void NonVolatileData::findSensIdHashOrEmpty(uint32_t sensorIdHash) {
	SensorTypeE2 idE2Data;
	_currAddress = ID_TABLE_START;
	idE2Data = iter();
	while (!dataIsEmpty(idE2Data)) {
		if (sensorIdHash == idE2Data.sensorIdHash) {
			// here, iter points to the next data entry
			_currAddress -= sizeof(SensorTypeE2);
			return;
		}
		idE2Data = iter();
	}
	return;
}
/**
 * If you want to iter through the id-table form outside, you have to call
 * this first, to set the current address pointer to the beginning of the
 * table.
 */
void NonVolatileData::startIter(void) {
	_currAddress = ID_TABLE_START;
}

/**
 * Reads contents at _currAddress, and increases _currAddress,
 * if content is not empty.
 */
NonVolatileData::SensorTypeE2 NonVolatileData::iter(void) {
	SensorTypeE2 idE2Data;
	AT24Cxxx::read(_currAddress, reinterpret_cast<uint8_t*>(&idE2Data),
			sizeof(SensorTypeE2));
	if (!dataIsEmpty(idE2Data)) {
		_currAddress += sizeof(SensorTypeE2);
	}
	return idE2Data;
}

Convert NonVolatileData::getConversion(SensorIdTable::SensorType sensType) {
	Convert conv;
	if (sensType == SensorIdTable::SensorType::TEMP) {
		conv.setLimits(convTheta);
	} else if (sensType == SensorIdTable::SensorType::HUMIDITY) {
		conv.setLimits(convHumid);
	} else if (sensType == SensorIdTable::SensorType::PRESS) {
		conv.setLimits(convPress);
	}
	return conv;
}

NonVolatileData::SensorTypeE2 NonVolatileData::physToE2(
		SensorIdTable::SensorIdType idPhysData) {
	SensorTypeE2 result;
	result.sensorIdHash = idPhysData.sensorIdHash;
	result.relayNr = idPhysData.relayNr;
	result.sensType = idPhysData.sensType;
	memcpy(result.shortname, idPhysData.shortname,
			SensorIdTable::SHORTNAME_LEN);

	Convert conv = getConversion(idPhysData.sensType);
	result.minVal = conv.phys2Int(idPhysData.minVal);
	result.maxVal = conv.phys2Int(idPhysData.maxVal);
	result.checkSum = calcChkSum(result);

	return result;
}

SensorIdTable::SensorIdType NonVolatileData::e2ToPhys(
		NonVolatileData::SensorTypeE2 idE2Data) {
	SensorIdTable::SensorIdType result;

	uint8_t chkSum = calcChkSum(idE2Data);
	if (chkSum != idE2Data.checkSum) {
		result.sensorIdHash = 0;
	} else {
		result.sensorIdHash = idE2Data.sensorIdHash;
	}
	result.relayNr = idE2Data.relayNr;
	result.sensType = static_cast<SensorIdTable::SensorType>(idE2Data.sensType);
	memcpy(result.shortname, idE2Data.shortname, SensorIdTable::SHORTNAME_LEN);

	Convert conv = getConversion(
			static_cast<SensorIdTable::SensorType>(idE2Data.sensType));
	result.minVal = conv.int2Phys(idE2Data.minVal);
	result.maxVal = conv.int2Phys(idE2Data.maxVal);

	return result;
}

bool NonVolatileData::compareIdTableDatum(SensorTypeE2 tableIDLeft,
		SensorTypeE2 tableIDRight) {

	uint8_t chkSumLeft = calcChkSum(tableIDLeft);
	uint8_t chkSumRight = calcChkSum(tableIDRight);

	// cross-check values
	if ((tableIDLeft.checkSum != chkSumRight) || (chkSumLeft != chkSumRight)
			|| (tableIDRight.checkSum != chkSumLeft)) {
		return false;
	}
	return true;
}

uint8_t NonVolatileData::calcChkSum(SensorTypeE2 idTableDatum) {
	idTableDatum.checkSum = 0;
	uint8_t hashCode = CrcSocket::calcChksum(
			reinterpret_cast<uint8_t*>(&idTableDatum), sizeof(SensorTypeE2));
	return hashCode;
}

ErrorCode NonVolatileData::writeStatId(uint32_t stationId) {
	uint32_t currentId = getStationId();
	if (currentId != stationId) {
		AT24Cxxx::write(STAT_ID_START, reinterpret_cast<uint8_t*>(&stationId),
				sizeof(uint32_t));
		return ERR_OK;
	}
	_statIdBuffered = currentId;
	return ERR_OK_E2_NOT_WRITTEN;
}

uint32_t NonVolatileData::getStationId(void) {
	if (_statIdBuffered == 0) {
		AT24Cxxx::read(STAT_ID_START,
				reinterpret_cast<uint8_t*>(&_statIdBuffered), sizeof(uint32_t));
	}
	return _statIdBuffered;
}

SensorIdTable::StationType NonVolatileData::getStationType(void) {
	uint32_t statId = getStationId();

	if (statId == 0) {
		return SensorIdTable::NONE;
	} else if (statId < 0xFF) {
		return SensorIdTable::MASTER;
	} else if (statId < 0x1FF) {
		return SensorIdTable::SLAVE_01;
	} else if (statId < 0x2FF) {
		return SensorIdTable::SLAVE_02;
	} else if (statId < 0x3FF) {
		return SensorIdTable::SLAVE_03;
	} else if (statId < 0x4FF) {
		return SensorIdTable::SLAVE_04;
	} else if (statId < 0x5FF) {
		return SensorIdTable::SLAVE_05;
	}

	return SensorIdTable::NONE;
}

bool NonVolatileData::dataIsEmpty(SensorTypeE2 idE2Data) {
	if (idE2Data.sensorIdHash == UINT32_MAX) {
		return true;
	}
	return false;
}

} /* namespace NonVolDat */
