/*
 * RaspySerial.h
 *
 *  Created on: 21.02.2022
 *      Author: harald
 */

#ifndef APPLICATION_RASPYSERIAL_RASPYSERIAL_H_
#define APPLICATION_RASPYSERIAL_RASPYSERIAL_H_

#include <Config/config.h>

namespace raspy {

class RaspySerial {
public:
	void init(void);
	static RaspySerial& instance(void);

	void cycle(void);

	void sendMessage(uint8_t* msg);

private:
	RaspySerial();
	virtual ~RaspySerial() {
	}
};

} /* namespace raspy */

#endif /* APPLICATION_RASPYSERIAL_RASPYSERIAL_H_ */
