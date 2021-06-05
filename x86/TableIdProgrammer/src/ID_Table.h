/*
 * ID_Table.h
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#ifndef THETASENSORS_ID_TABLE_H_
#define THETASENSORS_ID_TABLE_H_

#include <stdint.h>
#include <string>

// copied from ThetaMonitorNetwork/Application/ThetaSensors

class ID_Table {
public:
	static constexpr uint16_t ID_TABLE_LEN = 22;
	static constexpr uint16_t SHORTNAME_LEN = 8;
	static constexpr uint16_t INVLD_TEMPERATURE = 255;
	static constexpr uint16_t MIN_TEMP_HYSTERESIS = 1;
	static constexpr uint16_t SENSOR_ID_LEN = 8;

	enum SensorType {
		TEMP = 0, HUMIDITY = 1, PRESS = 2
	};
	enum StationType {
		MASTER = 3, SLAVE_01 = 2, SLAVE_02 = 1, SLAVE_03 = 0
	};

	typedef struct {
		uint32_t sensorIdHash;
		float minVal;
		float maxVal;
		SensorType sensType;
		uint8_t relayNr; // 0 = no relay
		char shortname[ID_Table::SHORTNAME_LEN];
	} Theta_sens_type; // to be used in application

	ID_Table();
	virtual ~ID_Table() {
	}
	;
	static void setSensId(Theta_sens_type sensVals);
	static void getSensIdTableE2(void);
	static void setStationId(uint32_t stationID);
	static const uint32_t getStationId(void);
	static std::string sensorType2Str(SensorType sensorType);
	static std::string sensorType2Str(uint8_t sensorType);

private:

};

#endif /* THETASENSORS_ID_TABLE_H_ */
