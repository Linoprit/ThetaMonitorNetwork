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

// we use this memory with placement-new for instantiating a RadioMessage
static char RadioMsgBuff[sizeof(RadioMessage<MeasurementType> )];

// https://www.windmill.co.uk/ascii-control-codes.html
// Ctrl	ASCII 	Dec 	Hex	Meaning
// ^Q 	DCL 	017 	11 	Xon (transmit on)
// ^B 	STX		002 	02 	Start of Text
// ^C 	ETX		003 	03 	End of Text
// ^S 	DC3 	019 	13	Xoff (transmit off)
uint8_t startBytes[] = { 17, 2 }; // serial msg start sequence
uint8_t tailBytes[] = { 3, 19 };  // serial msg stop sequence

RaspySerial::RaspySerial() {
	// Init RASPY_UART, we don't have receptions
	//HAL_UART_RxCpltCallback(&RASPY_UART); // init serial reception mechanism
}

void RaspySerial::init(void) {
	new (&instance()) RaspySerial();
}

RaspySerial& RaspySerial::instance(void) {
	static RaspySerial raspySerial;
	return raspySerial;
}

void RaspySerial::cycle(void) {

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

		sendMessage(radioMessage->asUint8Ptr());
	}

	// send remote sensors
	while (1) {
		uint8_t nRF24_payload[RADIO_MESSAGE_LEN];

		osStatus_t stat = osMessageQueueGet(remoteDataQueueHandle,
				&nRF24_payload, NULL, 0);

		if (stat == osOK) {
			sendMessage(nRF24_payload);
		} else {
			break; // no message? Leave.
		}
	}
}

void RaspySerial::sendMessage(uint8_t *msg) {
	// we do the same, like RadioSlave::sendMeasurements and sendStatistics
	uint8_t msg_length = static_cast<uint8_t>(RADIO_MESSAGE_LEN);
	tx_printBuff(startBytes, 2);
	tx_printBuff(&msg_length, 1);
	tx_printBuff(msg, RADIO_MESSAGE_LEN);
	tx_printBuff(tailBytes, 2);
}

} /* namespace raspy */
