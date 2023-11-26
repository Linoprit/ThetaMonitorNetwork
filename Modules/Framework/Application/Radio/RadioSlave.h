	/*
 * RadioLink.h
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#ifndef APPLICATION_RADIOLINK_RADIOLINK_H_
#define APPLICATION_RADIOLINK_RADIOLINK_H_

#include <Application/Radio/RadioBase.h>
#include <Application/Radio/RadioMaster.h>
#include <Middleware/NRF24L01_Radio/NRF24L01_Basis.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Libraries/SimpleQueue.h>
#include <array>
//#include <Application/RadioLink/Messages.h>

#define	 PACKED	__attribute__ ((packed))

namespace radio {
using namespace snsrs;

class RadioSlave: public RadioBase {
public:
	void init(void);
	static RadioSlave& instance(void);
	void cycle(void);

	uint32_t getTransmitCycleTime(void) {
		return _transmitCycleTime;
	}

private:
	RadioSlave();
	virtual ~RadioSlave() {
	}
	uint32_t _transmitCycleTime;

	void sendMeasurements(void);
	void sendStatistics(void);
};

} /* namespace radioLink */

#endif /* APPLICATION_RADIOLINK_RADIOLINK_H_ */
