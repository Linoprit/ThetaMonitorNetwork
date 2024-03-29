/*
 * RadioLink.cpp
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#include "RadioSlave.h"
#include <Application/Sensors/ThetaSensors.h>
#include <System/OsHelpers.h>
#include <new>
#include <System/serialPrintf.h>

namespace radio {

using namespace snsrs;

// we use this memory with placement-new for instanciating a RadioMessage
static char RadioMsgBuff[sizeof(RadioMessage<MeasurementType> )];

RadioSlave::RadioSlave() :
		_transmitCycleTime { MAXRT_TX_CYCLE_TIME } {
}

void RadioSlave::init(void) {
	new (&instance()) RadioSlave();
}

RadioSlave& RadioSlave::instance(void) {
	static RadioSlave radioSlave;
	return radioSlave;
}

void RadioSlave::cycle() {
	sendMeasurements();
	sendStatistics();
}

void RadioSlave::sendMeasurements(void) {
	uint8_t sensorCount =
			Sensors::instance().getThetaSensors()->getMeasurements()->getValidCount();

	// we send all found sensors, even if they timed out
	for (uint8_t i = 0; i < sensorCount; i++) {
		if(osSemaphoreAcquire(localMsmntSemHandle, 0) != osOK){
			continue;
		}
		ThetaMsmnt::MeasurementArray *measurementArray =
				Sensors::instance().getThetaSensors()->getMeasurementArray();
		RadioMsmntType *radioMessage = new (RadioMsgBuff) RadioMsmntType(
				MsgClass::MEASUREMENT, measurementArray->at(i));
		osSemaphoreRelease(localMsmntSemHandle);

		_nRF24L01_Basis.transmitPacket_IRQ(radioMessage->asUint8Ptr(),
				RADIO_MESSAGE_LEN);
		while (_nRF24L01_Basis.isTxOngoing()) {
			OsHelpers::delay(1);
		} // measured: 1.64ms

		// adjust send-cycle-time
		if (_nRF24L01_Basis.lastTxWasSuccess()) {
			_transmitCycleTime = STD_TX_CYCLE_TIME;
		} else {
			_transmitCycleTime = MAXRT_TX_CYCLE_TIME;
		}
	}
}

void RadioSlave::sendStatistics(void) {
	radio::RadioStatisticsType radiostats;
	radiostats.lostPkgs = _nRF24L01_Basis.get_lostPkgCount();
	// Todo relaystates are sent as an own measurement now.
	radiostats.relayStates = 0; //Sensors::instance().getRelayStates();
	radiostats.rxBufferOverflows = _nRF24L01_Basis.get_rxBufferOverflows();
	radiostats.stationId =
			Sensors::instance().getNonVolatileData()->getStationId();
	radiostats.validSensors =
			Sensors::instance().getThetaSensors()->getFoundDS1820();
	radiostats.lastUpdateTick = OsHelpers::get_tick_seconds();

	RadioStatMsgType *radioStatistics = new (RadioMsgBuff) RadioStatMsgType(
			MsgClass::STATISTICS, radiostats);

	_nRF24L01_Basis.transmitPacket_IRQ(radioStatistics->asUint8Ptr(),
			RADIO_MESSAGE_LEN);
	while (_nRF24L01_Basis.isTxOngoing()) {
		OsHelpers::delay(1);
	} // measured: 1.64ms

	if (_nRF24L01_Basis.lastTxWasSuccess()) {
		_nRF24L01_Basis.resetStatistics();
	}
}

} /* namespace radioLink */
