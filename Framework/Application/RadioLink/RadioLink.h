/*
 * RadioLink.h
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#ifndef APPLICATION_RADIOLINK_RADIOLINK_H_
#define APPLICATION_RADIOLINK_RADIOLINK_H_

#include <Application/RadioLink/NRF24L01_Basis.h>
#include <Application/ThetaSensors/ThetaMeasurement.h>
#include <Libraries/SimpleQueue.h>
#include <array>
//#include <Application/RadioLink/Messages.h>

#define	 PACKED	__attribute__ ((packed))

namespace radioLink {

class RadioLink {
public:
	// The types, defined here, must fit into the payload.
	// Refer to RadioMessage::PAYLOAD_LEN.
	typedef struct PACKED {
		uint32_t stationId;
		uint8_t relayStates; // bitfield
		uint8_t lostPkgs; // per hour
		uint8_t validSensors;
		uint8_t rxBufferOverflows; // in 24h
	} RadioStatisticType;

	void init(void);
	static RadioLink& instance(void);
	void initHardware(void);
	void cycle(void);

	NRF24L01_Basis* getNRF24L01_Basis(void) {
		return &nRF24L01_Basis;
	}

private:
	RadioLink() {
	}
	virtual ~RadioLink() {
	}
	NRF24L01_Basis nRF24L01_Basis;

};

} /* namespace radioLink */

#endif /* APPLICATION_RADIOLINK_RADIOLINK_H_ */
