/*
 * RadioLink.cpp
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#include <Application/RadioLink/RadioLink.h>
#include <Application/ThetaSensors/ThetaMeasurement.h>
#include <System/OsHelpers.h>
#include <new>
#include <System/serialPrintf.h>

namespace radioLink {

void RadioLink::init(void) {
	new (&instance()) RadioLink();
}

void RadioLink::initHardware() {
	OsHelpers::delay(200);
	nRF24L01_Basis.init();
}

RadioLink& RadioLink::instance(void) {
	static RadioLink radioLink;
	return radioLink;
}

void RadioLink::cycle() {

	static uint32_t count = 0;
	static RadioMessage<msmnt::ThetaMeasurement::MeasurementType> radioMessage;
	radioMessage.getPayload().sensorIdHash = count++;
	radioMessage.prepare();

	nRF24L01_Basis.transmitPacket(radioMessage.getMessagePtr(),
			RADIO_MESSAGE_LEN);
	while (nRF24L01_Basis.getLastTxResult() == NRF24L01::nRF24_TX_IS_ONGOING) {
		OsHelpers::delay(1);
	} // measured: 1.64ms
	NRF24L01::nRF24_TXResult result = nRF24L01_Basis.getLastTxResult();

	if (result == NRF24L01::nRF24_TX_ERROR) {
		tx_printf("nRF24_TX_ERROR\n");
	} else if (result == NRF24L01::nRF24_TX_SUCCESS) {
		tx_printf("nRF24_TX_SUCCESS\n");
	} else if (result == NRF24L01::nRF24_TX_TIMEOUT) {
		tx_printf("nRF24_TX_TIMEOUT\n");
	} else if (result == NRF24L01::nRF24_TX_MAXRT) {
		tx_printf("nRF24_TX_MAXRT\n");
	} else {
		tx_printf("nRF24_TX_UNKNOWN\n");
	}

	//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

} /* namespace radioLink */
