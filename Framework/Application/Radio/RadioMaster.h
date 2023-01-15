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
	void cycleLocal(void);
	void cycleRemote();
	void sendMessageSerial(uint8_t* msg);
	void setShutup(bool value){_shutup = value;}

private:
	RadioMaster();
	virtual ~RadioMaster() {
	}
	bool _shutup;
};

} /* namespace gateway */

#endif /* APPLICATION_RADIO_RADIOMASTER_H_ */
