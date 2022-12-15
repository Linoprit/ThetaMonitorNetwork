/*
 * Gateway.h
 *
 *  Created on: Jan 7, 2022
 *      Author: harald
 */

#ifndef APPLICATION_RADIO_RADIOMASTER_H_
#define APPLICATION_RADIO_RADIOMASTER_H_

#include <Application/Radio/RadioStatistics.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/Sensors/Measurements.h>

namespace radio {
using namespace snsrs;

// TODO not very satisfied to have them defined here...
// better put them in some central header.
// TODO check, if we can replace this structure with smaller queues
typedef RadioStatistics<MAX_SLAVES> RemoteRadioStatistics;
typedef Measurements<MAX_REMOTE_MEASUREMENTS> RemoteMsmnt;

class RadioMaster {
public:
	void init(void);
	static RadioMaster& instance(void);
	void initHardware(void);
	void cycle(void);

	void checkRadioBuffer(void);
	void enqueRxMessage(uint8_t *nRF24_payload);
//	RemoteMsmnt* getRemoteMsmnt(void) {
//		return &_remoteMsmnt;
//	}
//	;
//	RemoteRadioStatistics* getRemoteRadioStatistics(void) {
//		return &_remoteStats;
//	}
//	;

private:
	RadioMaster();
	virtual ~RadioMaster() {
	}

	// measurement of the sensors, sent by the nRF-Modules of the slaves.
	//RemoteMsmnt _remoteMsmnt;
	//RemoteRadioStatistics _remoteStats;

};

} /* namespace gateway */

#endif /* APPLICATION_RADIO_RADIOMASTER_H_ */
