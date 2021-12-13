/*
 * RadioLink.h
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#ifndef APPLICATION_RADIOLINK_RADIOLINK_H_
#define APPLICATION_RADIOLINK_RADIOLINK_H_

#include <Application/RadioLink/NRF24L01_Basis.h>
#include <Libraries/SimpleQueue.h>
//#include <Application/RadioLink/Messages.h>

#define	 PACKED	__attribute__ ((packed))

namespace radioLink {

class RadioLink {
public:

	typedef struct PACKED {
		uint32_t addressHash;
		float measurement;
		uint32_t timeEpoch;
	} SensorTypeRadio;



	RadioLink();
	virtual ~RadioLink() {
	}
	;
	void init(void);
	static RadioLink& instance(void);
	void cycle(void);

	NRF24L01_Basis& getNRF24L01_Basis(void) {
		return nRF24L01_Basis;
	}
	;

private:
	NRF24L01_Basis nRF24L01_Basis;
	//SimpleQueue<SensorTypeRadio, MAX_SENSORS> txSensors;

};

} /* namespace radioLink */

#endif /* APPLICATION_RADIOLINK_RADIOLINK_H_ */
