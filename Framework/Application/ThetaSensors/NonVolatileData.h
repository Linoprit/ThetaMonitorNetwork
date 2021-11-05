/*
 * NonVolatileData.h
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_APPLICATION_THETASENSORS_NONVOLATILEDATA_H_
#define FRAMEWORK_APPLICATION_THETASENSORS_NONVOLATILEDATA_H_

#include <Config/Types/AppTypes.h>
#include <Application/ThetaSensors/ID_Table.h>
#include <Libraries/Convert.h>

namespace msmnt {

#define	 PACKED	__attribute__ ((packed))

// Remember: invalid-value is uint8_t_max = 255!
constexpr ConvLimits convTheta { 0.1F, -12.7F, -12.7F, 12.7F };
constexpr ConvLimits convHumid { 0.156862745F, 40.0F, 40.0F, 80.0F };
constexpr ConvLimits convPress { 1.0F, 846.0F, 846.0F, 1100.0F };

class NonVolatileData {
public:
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
		char shortname[ID_Table::SHORTNAME_LEN];
		uint8_t checkSum;
	} Theta_sens_typeE2;

	void startIter(void);
	Theta_sens_typeE2 iter(void);
	ID_Table::Theta_sens_type getIdTableData(uint32_t sensorIdHash);
	ErrorCode clrIdTableData(void);
	ErrorCode writeIdTableData(ID_Table::Theta_sens_type sensVals);
	ErrorCode writeStatId(uint32_t stationId);
	uint32_t getStatId(void);
	static uint8_t calcChkSum(Theta_sens_typeE2 idTableDatum);
	bool compareIdTableDatum(Theta_sens_typeE2 tableIDLeft,
			Theta_sens_typeE2 tableIDRight);
	static Theta_sens_typeE2 physToE2(ID_Table::Theta_sens_type idSensValue);
	static ID_Table::Theta_sens_type e2ToPhys(Theta_sens_typeE2 e2Data);
	static Convert getConversion(ID_Table::SensorType sensType);
	void findSensIdHashOrEmpty(uint32_t sensorIdHash);
	void printIdTableRaw(void);
	bool dataIsEmpty(Theta_sens_typeE2 idE2Data);


private:
	static constexpr uint16_t STAT_ID_START = 12U;
	static constexpr uint16_t ID_TABLE_START = 64U;
	static constexpr uint16_t NUM_OF_ID_ENTRIES = 100U;
	static constexpr uint16_t ID_TABLE_LEN = NUM_OF_ID_ENTRIES
			* sizeof(ID_Table::Theta_sens_type);
	static constexpr uint32_t EMPTY_SENS_ID = UINT32_MAX;
	uint16_t _currAddress;
	uint16_t _oldAddress;
};

} /* namespace NonVolDat */

#endif /* FRAMEWORK_APPLICATION_THETASENSORS_NONVOLATILEDATA_H_ */
