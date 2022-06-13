/*
 * Gateway.h
 *
 *  Created on: Jan 7, 2022
 *      Author: harald
 */

#ifndef APPLICATION_RADIOGATEWAY_RADIOGATEWAY_H_
#define APPLICATION_RADIOGATEWAY_RADIOGATEWAY_H_

#include <Application/Sensors/ThetaSensors.h>
#include <Application/Sensors/Measurements.h>
#include <Application/RadioGateway/RadioStatistics.h>

namespace gate {
using namespace snsrs;

// TODO not very satisfied to have them defined here...
// better put them in some central header.
typedef RadioStatistics<MAX_SLAVES> RemoteRadioStatistics;
typedef Measurements<MAX_REMOTE_MEASUREMENTS> RemoteMsmnt;

class RadioGateway {
public:
	RadioGateway();
	virtual ~RadioGateway() {
	}

	void cycle();
	void checkRadioBuffer(void);
	void storeRxMessage(uint8_t *nRF24_payload);
	RemoteMsmnt* getRemoteMsmnt(void){ return &_remoteMsmnt; };
	RemoteRadioStatistics* getRemoteRadioStatistics(void){ return &_remoteStats; };


private:
	// measurement of the sensors, sent by the nRF-Modules of the slaves.
	RemoteMsmnt _remoteMsmnt;
	RemoteRadioStatistics _remoteStats;

};

} /* namespace gateway */

#endif /* APPLICATION_RADIOGATEWAY_RADIOGATEWAY_H_ */
