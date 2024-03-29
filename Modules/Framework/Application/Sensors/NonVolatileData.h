/*
 * NonVolatileData.h
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_APPLICATION_THETASENSORS_NONVOLATILEDATA_H_
#define FRAMEWORK_APPLICATION_THETASENSORS_NONVOLATILEDATA_H_

#include <Application/Sensors/SensorIdTable.h>
#include <Config/Types/AppTypes.h>
#include <Libraries/Convert.h>

namespace snsrs {

#define	 PACKED	__attribute__ ((packed))

// Remember: invalid-value is uint8_t_max = 255!
constexpr ConvLimits convTheta { 0.1F, -12.7F, -12.7F, 12.7F };
constexpr ConvLimits convHumid { 0.156862745F, 40.0F, 40.0F, 80.0F };
constexpr ConvLimits convPress { 1.0F, 846.0F, 846.0F, 1100.0F };


class NonVolatileData {
public:
	static constexpr uint32_t EMPTY_SENSOR_HASH = UINT32_MAX;

	NonVolatileData();
	virtual ~NonVolatileData() {
	}
	;
	typedef struct PACKED {
		uint32_t sensorIdHash;
		uint8_t minVal;
		uint8_t maxVal;
		uint8_t sensType :4;
		uint8_t relayNr :4; // 0 = no relay
		char shortname[SensorIdTable::SHORTNAME_LEN];
		uint8_t checkSum;
	} SensorTypeE2;

	void startIter(void);
	SensorTypeE2 iter(void);
	SensorIdTable::SensorIdType getIdTableData(uint32_t sensorIdHash);
	ErrorCode clrIdTableData(void);
	ErrorCode writeIdTableData(SensorIdTable::SensorIdType sensVals);
	ErrorCode writeStatId(uint32_t stationId);
	uint32_t getStationId(void);
	SensorIdTable::StationType getStationType(void);
	std::string getStationTypeStr(void);
	static uint8_t calcChkSum(SensorTypeE2 idTableDatum);
	bool compareIdTableDatum(SensorTypeE2 tableIDLeft,
			SensorTypeE2 tableIDRight);
	static SensorTypeE2 physToE2(SensorIdTable::SensorIdType idSensValue);
	static SensorIdTable::SensorIdType e2ToPhys(SensorTypeE2 e2Data);
	static Convert getConversion(SensorIdTable::SensorType sensType);
	void findSensIdHashOrEmpty(uint32_t sensorIdHash);
	void printIdTableRaw(void);
	bool dataIsEmpty(SensorTypeE2 idE2Data);

private:
	// EEPROM-Addresses
	static constexpr uint16_t STAT_ID_START = 12U;
	static constexpr uint16_t ID_TABLE_START = 64U;
	static constexpr uint16_t NUM_OF_ID_ENTRIES = 100U;
	static constexpr uint16_t ID_TABLE_LEN = NUM_OF_ID_ENTRIES
			* sizeof(SensorIdTable::SensorIdType);

	uint16_t _currAddress;
	uint16_t _oldAddress;
	uint32_t _statIdBuffered;
};

} /* namespace NonVolDat */

#endif /* FRAMEWORK_APPLICATION_THETASENSORS_NONVOLATILEDATA_H_ */
