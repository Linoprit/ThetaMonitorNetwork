/*
 * SerialIO.h
 *
 *  Created on: 25.05.2021
 *      Author: harald
 */

#ifndef SERIALIO_H_
#define SERIALIO_H_

#include <Framework/ProgramOptions.h>
// taken from: https://github.com/fedetft/serial-port
// thanks to fedetft
#include "BufferedAsyncSerial.h"

class SerialIO {
public:
	SerialIO(po::variables_map varMap);
	virtual ~SerialIO() { } ;

	void close(void);
	void open(void);
	int transmitCommands(vector<string> commands);
	int transmitCommand(string command);
	int readString(stringstream& receivedMsg);
	int writeString(string msg);

private:
	po::variables_map _varMap;
	BufferedAsyncSerial _serial;
};

#endif /* SERIALIO_H_ */
