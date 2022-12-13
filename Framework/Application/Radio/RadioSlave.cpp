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
static char RadioMsgBuff[sizeof(RadioMessage<MeasurementType>)];

RadioSlave::RadioSlave() :
		_transmitCycleTime { MAXRT_TX_CYCLE_TIME } {
}

void RadioSlave::init(void) {
	new (&instance()) RadioSlave();
}

void RadioSlave::initHardware() {
	OsHelpers::delay(200);
	_nRF24L01_Basis.init();
}

RadioSlave& RadioSlave::instance(void) {
	static RadioSlave radioSlave;
	return radioSlave;
}

void RadioSlave::cycle() {
	sendMeasurements();
	//OsHelpers::delay(100);
	//sendStatistics();


	// TODO  testcode, delete later
	//	static uint32_t count = 0;
	//
	//	radioMessage->getPayload().sensorIdHash = count++;
	//	radioMessage->prepare();
	//
	//	_nRF24L01_Basis.transmitPacket(radioMessage->getMessagePtr(),
	//			RADIO_MESSAGE_LEN);
	//	while (_nRF24L01_Basis.getLastTxResult() == NRF24L01::nRF24_TX_IS_ONGOING) {
	//		OsHelpers::delay(1);
	//	} // measured: 1.64ms
	//	NRF24L01::nRF24_TXResult result = _nRF24L01_Basis.getLastTxResult();
	//
	//	if (result == NRF24L01::nRF24_TX_ERROR) {
	//		tx_printf("nRF24_TX_ERROR\n");
	//	} else if (result == NRF24L01::nRF24_TX_SUCCESS) {
	//		tx_printf("nRF24_TX_SUCCESS\n");
	//	} else if (result == NRF24L01::nRF24_TX_TIMEOUT) {
	//		tx_printf("nRF24_TX_TIMEOUT\n");
	//	} else if (result == NRF24L01::nRF24_TX_MAXRT) {
	//		tx_printf("nRF24_TX_MAXRT\n");
	//	} else {
	//		tx_printf("nRF24_TX_UNKNOWN\n");
	//	}
	//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

// TODO send only measurement, that are valid (not timed-out)
void RadioSlave::sendMeasurements(void) {
	//RadioMsmntType *radioMessage = new (RadioMsgBuff) RadioMsmntType;

	uint8_t sensorCount =
			Sensors::instance().getThetaSensors()->getMeasurements()->getValidCount();

	// we send all found sensors, even if they timed out
	for (uint8_t i = 0; i < sensorCount; i++) {
		osSemaphoreAcquire(localMsmntSemHandle, 0);
		// ToDo clean, if it works
		// MeasurementType payload = radioMessage->getPayload();
		// payload = measurementArray->at(i);
		//radioMessage->prepare(MsgClass::MEASUREMENT);

		ThetaMsmnt::MeasurementArray *measurementArray =
		Sensors::instance().getThetaSensors()->getMeasurementArray();

		RadioMsmntType *radioMessage = new (RadioMsgBuff) RadioMsmntType(
				MsgClass::MEASUREMENT, measurementArray->at(i));

		osSemaphoreRelease(localMsmntSemHandle);

		_nRF24L01_Basis.transmitPacket(radioMessage->asUint8Ptr(),
				RADIO_MESSAGE_LEN);
		while (_nRF24L01_Basis.isTxOngoing()) {
			OsHelpers::delay(1);
		} // measured: 1.64ms

		// adjust send-cycle-time
		if (_nRF24L01_Basis.lastRxWasSuccess()) {
			_transmitCycleTime = STD_TX_CYCLE_TIME;
		} else {
			_transmitCycleTime = MAXRT_TX_CYCLE_TIME;
		}
	}
}

void RadioSlave::sendStatistics(void) {
	radio::RadioStatisticsType radiostats;
	radiostats.lostPkgs = _nRF24L01_Basis.get_lostPkgCount();
	radiostats.relayStates = Sensors::instance().getRelayStates();
	radiostats.rxBufferOverflows = _nRF24L01_Basis.get_rxBufferOverflows();
	radiostats.stationId =
			Sensors::instance().getNonVolatileData()->getStationId();
	radiostats.validSensors =
			Sensors::instance().getThetaSensors()->getFoundDS1820();

	RadioStatMsgType *radioStatistics = new (RadioMsgBuff) RadioStatMsgType(
			MsgClass::STATISTICS, radiostats);

	_nRF24L01_Basis.transmitPacket(radioStatistics->asUint8Ptr(),
			RADIO_MESSAGE_LEN);
	while (_nRF24L01_Basis.isTxOngoing()) {
		OsHelpers::delay(1);
	} // measured: 1.64ms

	if (_nRF24L01_Basis.lastRxWasSuccess()) {
		_nRF24L01_Basis.resetStatistics();
	}
}

} /* namespace radioLink */
