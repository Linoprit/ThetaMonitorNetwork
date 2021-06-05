/*
 * SimulationTask.cpp
 *
 *  Created on: 01.06.2021
 *      Author: harald
 */

#include <Tasks/SimulationTask.h>
#include <System/OsHelpers.h>
#include <boost/date_time.hpp>
#include <time.h>
#include <SerialIO/SerialIO.h>
#include <System/CommandLine/CommandLine.h>

// we must not include SerialIO.h in serialPrintf.h because some conflicting
// defines in termios.h
int tx_cycle(SerialIO *serialIO);

void StartSimulationTask(variables_map varMap) {
	SerialIO serialIO(varMap);
	uint8_t buffer;

	boost::posix_time::ptime startTime =
			boost::posix_time::microsec_clock::local_time();

	for (;;) {
		// service the systemTick
		boost::posix_time::ptime timeLocal =
				boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration diff = timeLocal - startTime;
		OsHelpers::setSystemTick(
				static_cast<uint32_t>(diff.total_milliseconds()));

		//service commandLine
		while (serialIO.receive(&buffer, 1) > 0) {
			cLine::CommandLine::instance().putChar(buffer);
			//cout << buffer << std::flush;
		}

		//output from application to serial-console
		tx_cycle(&serialIO);

		OsHelpers::delay(20);
	}

}

