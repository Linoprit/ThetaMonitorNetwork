
/*
 * Gateway.cpp
 *
 *  Created on: Jan 7, 2022
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#elif defined STM32F401xC ||  defined STM32F401xE
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#endif
#include <Application/Radio/RadioMaster.h>
#include <Application/Sensors/NonVolatileData.h>
#include <Application/Radio/RadioSlave.h>
#include <Config/config.h>
#include <System/serialPrintf.h>
#include <new>

namespace radio {

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

RadioMaster::RadioMaster() {
}

void RadioMaster::init(void) {
	new (&instance()) RadioMaster();
}

RadioMaster& RadioMaster::instance(void) {
	static RadioMaster radioMaster;
	return radioMaster;
}

void RadioMaster::cycle() {
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

		sendMessageSerial(radioMessage->asUint8Ptr());
	}
	// send remote sensors
	while (osMessageQueueGetCount(remoteDataQueueHandle) > 0) {
		uint8_t nRF24_payload[RADIO_MESSAGE_LEN];

		osStatus_t stat = osMessageQueueGet(remoteDataQueueHandle,
				&nRF24_payload, NULL, 0);

		if (stat == osOK) {
			sendMessageSerial(nRF24_payload);
		} else {
			break; // no message? Leave.
		}
	}
}

void RadioMaster::sendMessageSerial(uint8_t *msg) {
	// we do the same, like RadioSlave::sendMeasurements and sendStatistics
	uint8_t msg_length = static_cast<uint8_t>(RADIO_MESSAGE_LEN);
	tx_printBuff(startBytes, 2);
	tx_printBuff(&msg_length, 1);
	tx_printBuff(msg, RADIO_MESSAGE_LEN);
	tx_printBuff(tailBytes, 2);
}


//void RadioMaster::checkRadioBuffer(void) {
//	uint8_t nRF24_payload[RADIO_MESSAGE_LEN];
//
//	//TODO Use RTOS-Queue
//	NRF24L01_Basis::RxBufferQueue *nrfRxQueue =
//			radio::RadioSlave::instance().getNRF24L01_Basis()->getRxBuffer();
//
//	osSemaphoreAcquire(nRF_RxBuffSemHandle, 0);
//	int16_t queueSize = nrfRxQueue->size();
//	osSemaphoreRelease(nRF_RxBuffSemHandle);
//
//	while (queueSize >= static_cast<uint16_t>(RADIO_MESSAGE_LEN)) {
//
//		osSemaphoreAcquire(nRF_RxBuffSemHandle, 0);
//		for (uint8_t i = 0; i < RADIO_MESSAGE_LEN; i++) {
//			nRF24_payload[i] = nrfRxQueue->dequeue();
//		}
//		queueSize = nrfRxQueue->size();
//		osSemaphoreRelease(nRF_RxBuffSemHandle);
//
//		enqueRxMessage(nRF24_payload);
//		tx_printf("got Message.\n");
//	}
//}

//void RadioMaster::enqueRxMessage(uint8_t *nRF24_payload) {
//	// after checksum is ok, we put all messages into the same queue.
//	// optimization: calculate the checksum on bare payload.
//	// there is no need to cast the messages.
//
//	MsgClass msgClass = static_cast<MsgClass>(nRF24_payload[0]);
//
//	if (msgClass == MEASUREMENT) {
//		RadioMsmntType *radioMsmnt =
//				reinterpret_cast<RadioMsmntType*>(nRF24_payload);
//		if (radioMsmnt->isChkSumOk()) {
//			osMessageQueuePut(remoteDataQueueHandle, radioMsmnt, 5, 2);
//		}
//	} else if (msgClass == STATISTICS) {
//		RadioStatMsgType *radioStat =
//				reinterpret_cast<RadioStatMsgType*>(nRF24_payload);
//		if(radioStat->isChkSumOk()){
//			osMessageQueuePut(remoteDataQueueHandle, radioStat, 5, 2);
//		}
//	} // other Msg-classes are not used, at the moment
//}


} /* namespace gateway */
