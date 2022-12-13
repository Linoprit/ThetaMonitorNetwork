
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
#include <new>

namespace radio {

RadioMaster::RadioMaster() :
		_remoteMsmnt { remoteMsmntSemHandle } {
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

	//TODO divide MASTER and SLAVE data
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

		storeRxMessage(nRF24_payload);
	}
}

void RadioMaster::storeRxMessage(uint8_t *nRF24_payload) {
	MsgClass msgClass = static_cast<MsgClass>(nRF24_payload[0]);

	if (msgClass == MEASUREMENT) {
		RadioMsmntType *radioMsmnt =
				reinterpret_cast<RadioMsmntType*>(nRF24_payload);
		if (radioMsmnt->isChkSumOk()) {
			MeasurementType measurement = radioMsmnt->getPayload();
			//TODO remove this class and use a queue, do not mix local and remote msmnt
			_remoteMsmnt.update(measurement.sensorIdHash, measurement.value);
		}
	} else if (msgClass == STATISTICS) {
		RadioStatMsgType *radioStat =
				reinterpret_cast<RadioStatMsgType*>(nRF24_payload);
		if(radioStat->isChkSumOk()){
			RadioStatisticsType stat = radioStat->getPayload();
			_remoteStats.update(stat);
		}
	} // other Msg-classes are not used, at the moment

}


} /* namespace gateway */
