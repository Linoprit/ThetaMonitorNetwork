/*
 * Gateway.h
 *
 *  Created on: Jan 7, 2022
 *      Author: harald
 */

#ifndef APPLICATION_RADIO_RADIOMASTER_H_
#define APPLICATION_RADIO_RADIOMASTER_H_

#include <Application/Radio/RadioBase.h>
#include <Application/Radio/RadioStatistics.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/Sensors/Measurements.h>

namespace radio {
using namespace snsrs;

class RadioMaster: public RadioBase {
public:
	void init(void);
	static RadioMaster& instance(void);
	void cycle(void);
	void sendMessageSerial(uint8_t* msg);

private:
	RadioMaster();
	virtual ~RadioMaster() {
	}
};

} /* namespace gateway */

#endif /* APPLICATION_RADIO_RADIOMASTER_H_ */
