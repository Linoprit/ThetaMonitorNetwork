/*
 * RadioLink.h
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#ifndef APPLICATION_RADIOLINK_RADIOLINK_H_
#define APPLICATION_RADIOLINK_RADIOLINK_H_

#include <Middleware/NRF24L01_Radio/NRF24L01_Basis.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/RadioGateway/RadioGateway.h>
#include <Libraries/SimpleQueue.h>
#include <array>
//#include <Application/RadioLink/Messages.h>

#define	 PACKED	__attribute__ ((packed))

namespace radio {

using namespace snsrs;
using namespace gate;

class RadioLink {
public:
	void init(void);
	static RadioLink& instance(void);
	void initHardware(void);
	void cycle(void);

	uint32_t getTransmitCycleTime(void) {
		return _transmitCycleTime;
	}
	NRF24L01_Basis* getNRF24L01_Basis(void) {
		return &_nRF24L01_Basis;
	}

private:
	RadioLink();
	virtual ~RadioLink() {
	}
	uint32_t _transmitCycleTime;
	NRF24L01_Basis _nRF24L01_Basis;

	void sendMeasurements(void);
	void sendStatistics(void);
};

} /* namespace radioLink */

#endif /* APPLICATION_RADIOLINK_RADIOLINK_H_ */
