/*
 * Sensors.h
 *
 *  Created on: 08.01.2022
 *      Author: harald
 */

#ifndef APPLICATION_SENSORS_SENSORS_H_
#define APPLICATION_SENSORS_SENSORS_H_

#include <Application/Sensors/NonVolatileData.h>
#include <Application/Sensors/SensorIdTable.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Config/config.h>

namespace snsrs {

class Sensors {
public:
	void init(void);
	void initHardware(void);
	static Sensors& instance(void);
	void cycle(void);

	void checkRelays(void);
	ThetaSensors* getThetaSensors(void){
		return &_thetaSensors;
	}
	SensorIdTable* getSensorIdTable(void) {
		return &_sensorIdTable;
	}
	NonVolatileData* getNonVolatileData(void) {
		return &_nonVolatileData;
	}
	void printMsmntArray(void);

private:
	Sensors();
	virtual ~Sensors() {
	}

	NonVolatileData _nonVolatileData;
	SensorIdTable _sensorIdTable;
	ThetaSensors _thetaSensors;

};

} /* namespace sensors */

#endif /* APPLICATION_SENSORS_SENSORS_H_ */
