/*
 * MeasureTask.cpp
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

//#include <Tasks/MeasureTask.h>
#include <System/OsHelpers.h>
#include <Application/ThetaSensors/ThetaMeasurement.h>

void startMeasurementTask(void){
	msmnt::ThetaMeasurement::instance().init();

	for (;;) {


		OsHelpers::delay(1000);
	}
}
