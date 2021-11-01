/*
 * ProgramOptions.cpp
 *
 *  Created on: 26.05.2021
 *      Author: harald
 */

#include "ProgramOptions.h"
#include <boost/test/unit_test.hpp>
#include <Tests/InternalTests.h>

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream &os, const vector<T> &v) {
	copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
	return os;
}

string programVersion =
		"TableIdProgrammer V0.1\nPart of the ThetaMonitorNetwork project.";

int ProgramOptions::init(int argc, char *argv[]) {
	checkIfTest(argc, argv);

	try {
		string config_file;
		string filename;
		uint32_t stationId;
		string idTableFileName;
		string serialDevice;
		uint baudrate;
		uint waitstates;
		string acknowledge;
		string setSensorId;
		string readSensorTable;
		string setStationId;
		string getStationId;

// @formatter:off
		// command line only
		po::options_description generic("Generic options");
		generic.add_options()
				("version,v", "print version string")
				("help,h", "produce help message")
				("config,c",
						po::value<string>(&config_file)->default_value("default.cfg"),
						"use specific configuration file.");

		// both command line and config file
		po::options_description config("Configuration");
		config.add_options()
				("idTableFile", po::value<string>(&idTableFileName),
						"Input ID-Table file, with plain sensor-addresses")
				("progid,p", "write ID-Table to device")
				("readid,r", "Read ID-Table to File")
				("setstatid", po::value<uint32_t>(&stationId),
						"<uint32> write station-ID to device")
				("getstatid", "read station-ID from device")
				("device,d", po::value<string>(&serialDevice),
						"Serial-device, i.E. /dev/ttyUSB0")
				("baudrate,b",
						po::value<uint>(&baudrate)->default_value(115200),
						"Baudrate")
				("waitstates",
						po::value<uint>(&waitstates)->default_value(20),
						"time to wait, after command was sent, in 500ms steps")
				 ("acknowledge",
						 po::value<string>(&acknowledge)->default_value("**DONE**"),
						 "Response string from device")
				("setSensorIdCmd", po::value<string>(&setSensorId),
						"Device's command to set sensor-values")
				("readSensorTableCmd", po::value<string>(&readSensorTable),
						"Device's command to read the sensor-id table")
				("setStationIdCmd", po::value<string>(&setStationId),
						"Device's command to write the station-id to E2")
				("getStationIdCmd", po::value<string>(&getStationId),
						"Device's command to get the device's current station-id")
				("test", "execute tests");

		// Hidden options, command line and config file
		po::options_description hidden("Hidden options");
		hidden.add_options()
				("input-file", po::value<vector<string> >(), "input file");
// @formatter:on

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		po::options_description config_file_options;
		config_file_options.add(config).add(hidden);

		po::options_description visible("Allowed options");
		visible.add(generic).add(config);

		po::positional_options_description p;
		p.add("input-file", -1);

		store(
				po::command_line_parser(argc, argv).options(cmdline_options).positional(
						p).run(), _vm);
		notify(_vm);

		ifstream ifs(config_file.c_str());
		if (!ifs) {
			cout << "can not open config file: " << config_file << "\n";
			return _FAIL_;
		} else {
			store(parse_config_file(ifs, config_file_options), _vm);
			notify(_vm);
		}

		if (_vm.count("help")) {
			cout << visible << "\n";
			exit(0);
		} else if (_vm.count("version")) {
			cout << programVersion << "\n";
			exit(0);
		}

	} catch (exception &e) {
		cout << e.what() << "\n";
		return 1;
	}
	return _SUCCESS_;
}

void ProgramOptions::checkIfTest(int argc, char *argv[]) {
	// for running the tests call programm like this:
	// ./TableIdProgrammer --test --run_test=testA --log_level=all
	if (argc <= 1)
		return;

	string argvStr(argv[1]);
	if (argvStr == "--test") {
		::boost::unit_test::unit_test_main(&init_function, argc - 1, &argv[1]);
		exit(0);
	}
}
