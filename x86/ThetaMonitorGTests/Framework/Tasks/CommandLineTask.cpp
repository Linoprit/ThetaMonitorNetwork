/*
 * CommandLineTask.cpp
 *
 *  Created on: 31.05.2021
 *      Author: harald
 */

#include <System/CommandLine/CommandLine.h>
#include <System/OsHelpers.h>


using namespace std;



void initCommandLine(void) {
	cLine::CommandLine::instance().init();
	OsHelpers::delay(500);
	cLine::CommandLine::instance().splash();
	// tx_cycle(); // does it work without this?
	OsHelpers::delay(500);
}

void startCommandLineTask(void) {


	initCommandLine();

	for (;;) {



		cLine::CommandLine::instance().cycle();



		OsHelpers::delay(10);
	}
}

