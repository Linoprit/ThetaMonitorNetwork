/*
 * Sensors.cpp
 *
 *  Created on: 08.01.2022
 *      Author: harald
 */

#include <Application/Sensors/Sensors.h>
#include <Libraries/HelpersLib.h>
#include <System/serialPrintf.h>

namespace snsrs {

Sensors::Sensors() {
}

void Sensors::init(void) {
	new (&instance()) Sensors();

	uint32_t stationId = _nonVolatileData.getStationId();
	_thetaSensors.init(stationId);
}

void Sensors::initHardware(void) {
	_thetaSensors.initHardware();
}

Sensors& Sensors::instance(void) {
	static Sensors sensors;
	return sensors;
}

void Sensors::cycle(void) {
	_thetaSensors.cycle();

	checkRelays();
}

void Sensors::checkRelays(void) {

	//SensorIdTable::SensorIdType NonVolatileData::getIdTableData(uint32_t sensorIdHash)

	// TODO checkRelays => _relayStates

}

void Sensors::printMsmntArray(void) {
	for (uint8_t i = 0; i < MAX_SENSORS; i++) {
		MeasurementType actSensor =
				_thetaSensors.getMeasurementArray()->at(i);
		std::string valueStr = HelpersLib::floatToStr(actSensor.value, 2u);

		SensorIdTable::SensorIdType sensorConfig = _sensorIdTable.getSensorId(
				&_nonVolatileData, actSensor.sensorIdHash);

		std::string shortname = std::string(sensorConfig.shortname, 8);
		std::string sensorTypeStr = SensorIdTable::sensorType2Str(
				sensorConfig.sensType);

		tx_printf("%s / %lu: %s '%s'\n", shortname.c_str(),
				actSensor.sensorIdHash, valueStr.c_str(),
				sensorTypeStr.c_str());
	}
	tx_printf("\n");
}

} /* namespace sensors */
