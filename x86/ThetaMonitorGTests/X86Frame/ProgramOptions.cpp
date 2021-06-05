/*
 * ProgramOptions.cpp
 *
 *  Created on: 26.05.2021
 *      Author: harald
 */

#include "ProgramOptions.h"
//#include <boost/test/unit_test.hpp>
//#include <Tests/InternalTests.h>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream &os, const vector<T> &v) {
	copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
	return os;
}

string programVersion =
		"ThetaMonitorGTests V0.1\nPart of the ThetaMonitorNetwork project.";

int ProgramOptions::init(int argc, char *argv[]) {
	checkIfTest(argc, argv);

	try {
		string config_file;
		string filename;
		string serialDevice;
		uint baudrate;
		uint waitstates;
		string acknowledge;

// @formatter:off
		// command line only
		po::options_description generic("Generic options");
		generic.add_options()
				("version,v", "print version string")
				("help,h", "produce help message")
				("config,c",
						po::value<string>(&config_file)->default_value("ThetaMonitorGTestsDefault.cfg"),
						"use specific configuration file.");

		// both command line and config file
		po::options_description config("Configuration");
		config.add_options()
				("test", "execute tests")
				("sim,s", "start simulation (do not run tests)")

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
				;
// @formatter:on

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config);

		po::options_description config_file_options;
		config_file_options.add(config);

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
			return FAIL;
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
	return SUCCESS;
}

void ProgramOptions::checkIfTest(int argc, char *argv[]) {
	// for running the tests call programm like this:
	// ./ThetaMonitorGTests --test --gtest_filter=test1*
	if (argc <= 1)
		return;

	string argvStr(argv[1]);
	if (argvStr == "--test") {
		::testing::InitGoogleTest(&argc, argv);
		int result = RUN_ALL_TESTS();
		if (result == 0) {
			cout << "All tests succeeded." << endl;
		} else {
			cout << "!Some tests failed!" << endl;
		}
		exit(0);
	}
}
