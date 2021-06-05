/*
 * SerialIO.h
 *
 *  Created on: 25.05.2021
 *      Author: harald
 */

#ifndef SERIALIO_H_
#define SERIALIO_H_

#include <ProgramOptions.h>
// taken from: https://github.com/fedetft/serial-port
// thanks to fedetft
#include "BufferedAsyncSerial.h"

class SerialIO {
public:
	SerialIO(po::variables_map varMap);
	virtual ~SerialIO() { } ;

	void close(void);
	void open(void);
	int transmit(uint8_t *buffer, uint16_t size);
	size_t receive(uint8_t *buffer, uint16_t size);


	int readString(stringstream& receivedMsg);
	int writeString(string msg);

private:
	po::variables_map _varMap;
	BufferedAsyncSerial _serial;
};

#endif /* SERIALIO_H_ */
