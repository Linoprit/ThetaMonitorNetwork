/*
 * Sensors.cpp
 *
 *  Created on: 08.01.2022
 *      Author: harald
 */

#include <Application/Sensors/Sensors.h>
#include <Libraries/HelpersLib.h>
#include <System/serialPrintf.h>
#include <System/Sockets/GPIOSocket_Relays.h>

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

/* Switches the relays on or off, regarding to their limits, set in the sensorIdTable
 * TODO: Discussion - should we add a maxTime for relay on? If a relay is switched on
 * and the corresponding sensor stops working, the relay stays on forever.
 * TODO avoid too frequent switching.
 */
void Sensors::checkRelays(void) {
	uint8_t sensorCount =
			Sensors::instance().getThetaSensors()->getMeasurements()->getValidCount();

	for (uint8_t i = 0; i < sensorCount; i++) {

		if(osSemaphoreAcquire(localMsmntSemHandle, 10) != osOK){
			continue;
		}
		MeasurementType actSensor = _thetaSensors.getMeasurementArray()->at(i);
		osSemaphoreRelease(localMsmntSemHandle);

		SensorIdTable::SensorIdType sensorConfig =
				_sensorIdTable.getSensorTableData(&_nonVolatileData,
						actSensor.sensorIdHash);
		if ((sensorConfig.sensorIdHash == 0) || // No data for sensor found on E2
				(sensorConfig.relayNr == 0) ||  // 0 means, no relay to service
				(!ThetaMsmnt::isValueValid(actSensor))) { // NAN means timeout
			continue;
		}

		if (actSensor.value < sensorConfig.minVal) {
			GPIOSocket_Relays::switch_relay_on(sensorConfig.relayNr);
		}
		if (actSensor.value > sensorConfig.maxVal) {
			GPIOSocket_Relays::switch_relay_off(sensorConfig.relayNr);
		}
	}
}

/* For sendStatistics. We have only a uint8-bitfield. So we ignore the possibility
 * of having more relays.
 * The relay_1 is at the very right side.
 * In example, if relay_1 is switched on and relay_2 is off, the returned value is
 * 00000001b = 1 dec
 */
uint8_t Sensors::getRelayStates(void) {
	uint8_t result = 0;
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t state = GPIOSocket_Relays::get_relay_state(i + 1);
		result = result | state << i;
	}
	return result;
}

/* For debugging, print the contents of the whole measurement-array.
 */
void Sensors::printMsmntArray(void) {
	for (uint8_t i = 0; i < MAX_SENSORS; i++) {
		MeasurementType actSensor = _thetaSensors.getMeasurementArray()->at(i);
		std::string valueStr = HelpersLib::floatToStr(actSensor.value, 2u);

		SensorIdTable::SensorIdType sensorConfig =
				_sensorIdTable.getSensorTableData(&_nonVolatileData,
						actSensor.sensorIdHash);

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
