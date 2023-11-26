/*
 * RadioBase.h
 *
 *  Created on: Dec 17, 2022
 *      Author: harald
 */

#ifndef APPLICATION_RADIO_RADIOBASE_H_
#define APPLICATION_RADIO_RADIOBASE_H_

#include <Middleware/NRF24L01_Radio/NRF24L01_Basis.h>

namespace radio {

class RadioBase {
public:
	RadioBase() {
	}
	virtual ~RadioBase() {
	}

	void initHardware(void) {
		OsHelpers::delay(200);
		_nRF24L01_Basis.init();
	}

	NRF24L01_Basis* getNRF24L01_Basis(void) {
		return &_nRF24L01_Basis;
	}
protected:
	NRF24L01_Basis _nRF24L01_Basis;

};

} /* namespace radio */

#endif /* APPLICATION_RADIO_RADIOBASE_H_ */
