
/*
 * dspCallbackTask.cpp
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#elif defined STM32F401xE
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#endif
#include <System/OsHelpers.h>
#include "tasksDef.h"
#include <Application/RadioLink/RadioLink.h>
#include <Application/Sensors/ThetaSensors.h>
#include <System/serialPrintf.h>

void startnRF24Task(void *argument) {
	UNUSED(argument);
//	radioLink::RadioLink::instance().init();
//	radioLink::RadioLink::instance().initHardware();

	for (;;) {
		// HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//		radioLink::RadioLink::instance().cycle();

//		OsHelpers::delay(
//				radioLink::RadioLink::instance().getTransmitCycleTime());

		OsHelpers::delay(1000);

		//
		//
//		msmnt::SensorIdTable::StationType stationType =
//				msmnt::ThetaMeasurement::instance().getNonVolatileData()->getStationType();
//
//		if (stationType == msmnt::SensorIdTable::StationType::SLAVE_01) {
//
//		} else if (stationType == msmnt::SensorIdTable::StationType::MASTER) {
//
//			bool isLocked =
//					radioLink::RadioLink::instance().getNRF24L01_Basis()->isRxBufLocked();
//			if (!isLocked) {
//				RadioMessage<msmnt::ThetaMeasurement::SensorMeasureType> radioMessage;
//
//				SimpleQueue<uint8_t, nRF24_RX_BUFFER_SIZE> *rxBuffer =
//						radioLink::RadioLink::instance().getNRF24L01_Basis()->getRxBuffer();
//
//				if (static_cast<std::size_t>(rxBuffer->size())
//						>= RADIO_MESSAGE_LEN) {
//					for (uint8_t i = 0; i < RADIO_MESSAGE_LEN; i++) {
//						radioMessage.getMessagePtr()[i] = rxBuffer->dequeue();
//					}
//
//					msmnt::ThetaMeasurement::SensorMeasureType radioMeasurement =
//							radioMessage.getPayload();
//					tx_printf("count: %lu / %i\n",
//							radioMeasurement.sensorIdHash,
//							radioMessage.isChkSumOk());
//				}
//			}
//		}
//		OsHelpers::delay(1000);
	}

}

