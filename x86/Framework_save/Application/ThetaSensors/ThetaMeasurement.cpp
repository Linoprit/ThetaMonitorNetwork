/*
 * ThetaMeasurement.cpp
 *
 *  Created on: 03.06.2021
 *      Author: harald
 */

#include <Application/ThetaSensors/ThetaMeasurement.h>
#include <new>

namespace msmnt {

void ThetaMeasurement::init(void) {
	new (&instance()) ThetaMeasurement();
}

ThetaMeasurement& ThetaMeasurement::instance(void) {
	static ThetaMeasurement thetaMeasurement;
	return thetaMeasurement;
}

ThetaMeasurement::ThetaMeasurement() {


}

void ThetaMeasurement::cycle(void) {


}

} // namespace msmnt
