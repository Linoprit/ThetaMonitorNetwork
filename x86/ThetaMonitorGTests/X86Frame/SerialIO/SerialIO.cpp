/*
 * SerialIO.cpp
 *
 *  Created on: 25.05.2021
 *      Author: harald
 */

#include "SerialIO.h"

#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

SerialIO::SerialIO(po::variables_map varMap) {
	cout << "Opening serial device: " << varMap["device"].as<string>() << "\n";
	_serial.open(varMap["device"].as<string>(), varMap["baudrate"].as<uint>());
	_varMap = varMap;
}

void SerialIO::close(void) {
	_serial.close();
}

void SerialIO::open(void) {
	cout << "Opening serial device: " << _varMap["device"].as<string>() << "\n";
	_serial.open(_varMap["device"].as<string>(),
			_varMap["baudrate"].as<uint>());
}

int SerialIO::transmit(uint8_t *buffer, uint16_t size) {
	char *buff = reinterpret_cast<char*>(buffer);
	_serial.write(const_cast<const char*>(buff), size);
	return _SUCCESS_;
}

size_t SerialIO::receive(uint8_t *buffer, uint16_t size) {
	char *buff = reinterpret_cast<char*>(buffer);
	return _serial.read(buff, size);
}

int SerialIO::writeString(string msg) {
	if (!_serial.isOpen()) {
		return _FAIL_;
	}
	try {
		_serial.writeString(msg);
	} catch (boost::system::system_error &e) {
		cout << "SerialIO error: " << e.what() << endl;
		return _FAIL_;
	}
	return _SUCCESS_;
}

int SerialIO::readString(stringstream &receivedMsg) {
	if (!_serial.isOpen()) {
		return _FAIL_;
	}
	try {
		receivedMsg << _serial.readString();
	} catch (boost::system::system_error &e) {
		cout << "SerialIO error: " << e.what() << endl;
		return _FAIL_;
	}
	return _SUCCESS_;
}

