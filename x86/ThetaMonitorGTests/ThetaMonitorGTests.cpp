#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <ProgramOptions.h>
#include <thread>
#include <Tasks/tasksDef.h>
#include <EepromEmulation.h>
#include <X86Tasks/SimulationTask.h>

using namespace boost::program_options;
using namespace std;

/**
 * For compiling, you need to define:
 * 	-DBOOST_TEST_ALTERNATIVE_INIT_API -DSTM32F103xB
 * And you need following libs:
 * 	boost_program_options / boost_thread / boost_chrono
 * 	gmock_main / gtest_main / gmock / gtest
 * And enable -lpthread
 */

void startSimulation(po::variables_map varMap) {
	EepromEmulation::init();

	std::thread simulationTask(StartSimulationTask, varMap);


	// start the simulation tasks
	std::thread commandLineTask(startMasterSerialTask);

	for (;;) {
		// you must end the program with ctrl-c
	}
	//	commandLineTask.join();
}

ProgramOptions progOptions;

int main(int argc, char **argv) {
	if (_FAIL_ == progOptions.init(argc, argv)) {
		return _FAIL_;
	}
	po::variables_map varMap = progOptions.getVarMap();

	if (varMap.count("sim")) {
		std::cout << "Starting simualtion..." << endl;
		startSimulation(varMap);
		return 0;
	} else {
		cout << "unknown command, type -h for help" << endl;
	}

	return 0;
}
