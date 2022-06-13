/*
 * RaspySerial.h
 *
 *  Created on: 21.02.2022
 *      Author: harald
 */

#ifndef APPLICATION_RASPYSERIAL_RASPYSERIAL_H_
#define APPLICATION_RASPYSERIAL_RASPYSERIAL_H_

namespace raspy {

class RaspySerial {
public:
	RaspySerial();
	virtual ~RaspySerial() {}

	void cycle(void);
};

} /* namespace raspy */

#endif /* APPLICATION_RASPYSERIAL_RASPYSERIAL_H_ */
