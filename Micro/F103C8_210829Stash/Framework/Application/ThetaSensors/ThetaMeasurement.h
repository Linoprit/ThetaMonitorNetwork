/*
 * ThetaMeasurement.h
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_
#define FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_

#include <Application/ThetaSensors/ID_Table.h>
#include <Application/ThetaSensors/NonVolatileData.h>

namespace msmnt {

class ThetaMeasurement {
public:
	ThetaMeasurement();
	virtual ~ThetaMeasurement() {
	}
	;

	void init(void);
	static ThetaMeasurement& instance(void);
	void cycle(void);

	ID_Table getIdTable(void) { return _idTable; }
	NonVolatileData getNonVolatileData(void) { return _nonVolatileData; }

private:
	ID_Table _idTable;
	NonVolatileData _nonVolatileData;

};

} // namespace msmnt

#endif /* FRAMEWORK_APPLICATION_THETASENSORS_THETAMEASUREMENT_H_ */
