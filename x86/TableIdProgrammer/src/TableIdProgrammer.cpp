//============================================================================
// Name        : TableIdProgrammer.cpp
// Author      : Harald
//============================================================================

#include <iostream>
#include <boost/program_options.hpp>
#include <iostream>

#include "ReadFile.h"
#include "IdTableParser.h"
#include <Framework/ProgramOptions.h>
#include <SerialIO/SerialIO.h>

using namespace boost::program_options;
using namespace std;

/**
 * For compiling, you need to define: -DBOOST_TEST_ALTERNATIVE_INIT_API
 */

// ToDo print response from all get* commands
ProgramOptions progOptions;

void finalMessage(int result) {
	if (result == _SUCCESS_) {
		cout << "SerialIO finished successfully." << endl;
	} else {
		cout << "Sorry, SerialIO failed." << endl;
	}
}

void sendCommand(po::variables_map varMap, vector<string> commands, bool printAnswer) {
	SerialIO serialIO(varMap);
	int result = serialIO.transmitCommands(commands, printAnswer);
	serialIO.close();
	finalMessage(result);
}

int main(int argc, char *argv[]) {
	if (_FAIL_ == progOptions.init(argc, argv)) {
		return _FAIL_;
	}
	po::variables_map varMap = progOptions.getVarMap();

	if (varMap.count("progid")) {
		std::cout << "Programming Id-file to device..." << endl;
		ReadFile readFile(varMap["idTableFile"].as<string>());
		IdTableParser idTableParser(varMap, readFile.getContent());
		sendCommand(varMap, idTableParser.getContent(), false);
		return 0;
	} else if (varMap.count("readid")) {
		cout << "Reading ID-Table from devive..." << endl;
		vector<string> commands;
		commands.push_back(varMap["readSensorTableCmd"].as<string>() + "\r\n");
		sendCommand(varMap, commands, true);
		return 0;
	} else if (varMap.count("setstatid")) {
		cout << "Setting station id..." << endl;
		vector<string> commands;
		//string statidStr = to_string(varMap["setStationIdCmd"].as<uint32_t>());
		string statidStr = to_string(varMap["setstatid"].as<uint>());
		commands.push_back(
				varMap["setStationIdCmd"].as<string>() + " " + statidStr
						+ "\r\n");
		sendCommand(varMap, commands, false);
		return 0;
	} else if (varMap.count("setStationIdCmd")) {
		cout << "Getting station id..." << endl;
		vector<string> commands;
		commands.push_back(varMap["getStationIdCmd"].as<string>() + "\r\n");
		sendCommand(varMap, commands, true);
		return 0;
	} else {
		cout << "unknown command, type -h for help" << endl;
	}

	return 0;
}
