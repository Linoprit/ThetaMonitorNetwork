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
#include <Application/RadioGateway/RadioGateway.h>
#include <Application/Sensors/NonVolatileData.h>
#include <Application/RadioLink/RadioLink.h>
#include <Config/config.h>
#include <new>

namespace gate {
using namespace radio;

RadioGateway::RadioGateway() :
		_remoteMsmnt { remoteMsmntSemHandle } {
}

void RadioGateway::cycle() {
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	checkRadioBuffer();
	// TODO pushToSerial
}

void RadioGateway::checkRadioBuffer(void) {
	uint8_t nRF24_payload[RADIO_MESSAGE_LEN];

	NRF24L01_Basis::RxBufferQueue *nrfRxQueue =
			radio::RadioLink::instance().getNRF24L01_Basis()->getRxBuffer();

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

void RadioGateway::storeRxMessage(uint8_t *nRF24_payload) {
	MsgClass msgClass = static_cast<MsgClass>(nRF24_payload[0]);

	if (msgClass == MEASUREMENT) {
		RadioLink::RadioMsmntType *radioMsmnt =
				reinterpret_cast<RadioLink::RadioMsmntType*>(nRF24_payload);
		if (radioMsmnt->isChkSumOk()) {
			MeasurementType measurement = radioMsmnt->getPayload();
			_remoteMsmnt.update(measurement.sensorIdHash, measurement.value);
		}
	} else if (msgClass == STATISTICS) {
		RadioLink::RadioStatMsgType *radioStat =
				reinterpret_cast<RadioLink::RadioStatMsgType*>(nRF24_payload);
		if(radioStat->isChkSumOk()){
			RadioStatisticsType stat = radioStat->getPayload();
			_remoteStats.update(stat);
		}
	} // other classes are not used, at the moment

}


} /* namespace gateway */
