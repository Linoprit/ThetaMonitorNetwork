
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

RadioMaster::RadioMaster() {
}

void RadioMaster::init(void) {
	new (&instance()) RadioMaster();
}

void RadioMaster::initHardware() {
	OsHelpers::delay(200);
	//TODO divide MASTER and SLAVE data
	radio::RadioSlave::instance().init();
}

RadioMaster& RadioMaster::instance(void) {
	static RadioMaster radioMaster;
	return radioMaster;
}

void RadioMaster::cycle() {
	checkRadioBuffer();
}

void RadioMaster::checkRadioBuffer(void) {
	uint8_t nRF24_payload[RADIO_MESSAGE_LEN];

	//TODO Use RTOS-Queue
	NRF24L01_Basis::RxBufferQueue *nrfRxQueue =
			radio::RadioSlave::instance().getNRF24L01_Basis()->getRxBuffer();

	osSemaphoreAcquire(nRF_RxBuffSemHandle, 0);
	int16_t queueSize = nrfRxQueue->size();
	osSemaphoreRelease(nRF_RxBuffSemHandle);

	while (queueSize >= static_cast<uint16_t>(RADIO_MESSAGE_LEN)) {

		osSemaphoreAcquire(nRF_RxBuffSemHandle, 0);
		for (uint8_t i = 0; i < RADIO_MESSAGE_LEN; i++) {
			nRF24_payload[i] = nrfRxQueue->dequeue();
		}
		queueSize = nrfRxQueue->size();
		osSemaphoreRelease(nRF_RxBuffSemHandle);

		enqueRxMessage(nRF24_payload);
		tx_printf("got Message.\n");
	}
}

void RadioMaster::enqueRxMessage(uint8_t *nRF24_payload) {
	// after checksum is ok, we put all messages into the same queue.
	// optimization: calculate the checksum on bare payload.
	// there is no need to cast the messages.

	MsgClass msgClass = static_cast<MsgClass>(nRF24_payload[0]);

	if (msgClass == MEASUREMENT) {
		RadioMsmntType *radioMsmnt =
				reinterpret_cast<RadioMsmntType*>(nRF24_payload);
		if (radioMsmnt->isChkSumOk()) {
			osMessageQueuePut(remoteDataQueueHandle, radioMsmnt, 5, 2);
		}
	} else if (msgClass == STATISTICS) {
		RadioStatMsgType *radioStat =
				reinterpret_cast<RadioStatMsgType*>(nRF24_payload);
		if(radioStat->isChkSumOk()){
			osMessageQueuePut(remoteDataQueueHandle, radioStat, 5, 2);
		}
	} // other Msg-classes are not used, at the moment

}


} /* namespace gateway */
