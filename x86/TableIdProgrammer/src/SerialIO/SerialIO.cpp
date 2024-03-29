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

//int SerialIO::transmitCommand(string command) {
//	uint count = 0;
//	stringstream answer;
//
//	cout << "SEND: " << command << endl;
//	writeString(command);
//
//	cout << "ANS = " << std::flush;
//	while ((count < _varMap["waitstates"].as<uint>())) {
//		if (readString(answer) == _FAIL_) {
//			return _FAIL_;
//		}
//		this_thread::sleep_for(chrono::milliseconds(500));
//		if (answer.str().size() == 0) {
//			this_thread::sleep_for(chrono::milliseconds(500));
//			count++;
//		} else {
//			cout << answer.str() << endl;
//			return _SUCCESS_;
//		}
//	}
//	cout << endl;
//	return _FAIL_;
//}

int SerialIO::transmitCommands(vector<string> commands, bool printAnswer) {
	for (string item : commands) {
		uint count = 0;
		bool ackOk = false;
		stringstream answer;

		cout << "SENDING: " << item << endl;
		writeString(item);

		while ((count < _varMap["waitstates"].as<uint>()) || (ackOk)) {
			if (readString(answer) == _FAIL_) {
				return _FAIL_;
			}

			string toFind = _varMap["acknowledge"].as<string>();
			if (answer.str().find(toFind) != std::string::npos) {
				ackOk = true;
				cout << "OK" << endl << endl;
				break;
			}
			this_thread::sleep_for(chrono::milliseconds(500));
			count++;
			cout << "." << std::flush;
		}

		if (ackOk != true) {
			return _FAIL_;
		} else {
			if (printAnswer) {
				cout << answer.str() << endl;
			}
		}
	}
	return _SUCCESS_;
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

