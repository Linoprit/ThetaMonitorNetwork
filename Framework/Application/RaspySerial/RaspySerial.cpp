/*
 * RaspySerial.cpp
 *
 *  Created on: 21.02.2022
 *      Author: harald
 */

#include <Application/RaspySerial/RaspySerial.h>
#include <Application/Radio/RadioStatistics.h>
#include <Application/Sensors/Sensors.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/Sensors/Measurements.h>
#include <Application/Radio/RadioMessage.h>
#include <System/serialPrintf.h>
#include <new>

namespace raspy {

using namespace snsrs;

// we use this memory with placement-new for instanciating a RadioMessage
static char RadioMsgBuff[sizeof(RadioMessage<MeasurementType>)];

RaspySerial::RaspySerial() {
	// Init RASPY_UART, we don't have receptions
	//HAL_UART_RxCpltCallback(&RASPY_UART); // init serial reception mechanism
}

void RaspySerial::init(void){
	new (&instance()) RaspySerial();
}

RaspySerial& RaspySerial::instance(void){
	static RaspySerial raspySerial;
	return raspySerial;
}

void RaspySerial::cycle(void){
	// we do the same, like RadioSlave::sendMeasurements and sendStatistics

	// https://www.windmill.co.uk/ascii-control-codes.html
	// Ctrl	ASCII 	Dec 	Hex	Meaning
	// ^Q 	DCL 	017 	11 	Xon (transmit on)
	// ^B 	STX		002 	02 	Start of Text
	// ^C 	ETX		003 	03 	End of Text
	// ^S 	DC3 	019 	13	Xoff (transmit off)
	static uint8_t startBytes[] = {17, 2};
	static uint8_t tailBytes[] = {3, 19};

	uint8_t sensorCount =
			Sensors::instance().getThetaSensors()->getMeasurements()->getValidCount();
	ThetaMsmnt::MeasurementArray *measurementArray =
			Sensors::instance().getThetaSensors()->getMeasurementArray();

	// we send all found sensors, even if they timed out
	for (uint8_t i = 0; i < sensorCount; i++) {
		osSemaphoreAcquire(localMsmntSemHandle, 0);
		RadioMsmntType *radioMessage = new (RadioMsgBuff) RadioMsmntType(
				MsgClass::MEASUREMENT, measurementArray->at(i));
		osSemaphoreRelease(localMsmntSemHandle);

		uint8_t msg_length = static_cast<uint8_t>(RADIO_MESSAGE_LEN);
		tx_printBuff(startBytes, 2);
		tx_printBuff(&msg_length, 1);
		tx_printBuff(radioMessage->asUint8Ptr(), RADIO_MESSAGE_LEN);
		tx_printBuff(tailBytes, 2);
	}

	// TODO send measurements to raspi-USART
	// ThetaMsmnt _measurementArray; // directly connected sensors
	// ThetaMsmnt* getMeasurements(void)
	//
	// RemoteMsmnt _remoteMsmnt; // nRF24 connected sensors
	// RemoteMsmnt* getRemoteMsmnt(void)
	// RemoteRadioStatistics* getRemoteRadioStatistics(void)
	//
}


} /* namespace raspy */
