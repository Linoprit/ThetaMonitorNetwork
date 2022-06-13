/*
 * RaspySerial.cpp
 *
 *  Created on: 21.02.2022
 *      Author: harald
 */

#include <Application/RaspySerial/RaspySerial.h>

namespace raspy {

RaspySerial::RaspySerial() {
	// TODO Auto-generated constructor stub

}
void RaspySerial::cycle(void){
	// we do the same, like RadioLink::sendMeasurements and sendStatistics

/*	RadioMsmntType *radioMessage = new (RadioMsgBuff) RadioMsmntType;

	uint8_t sensorCount =
			Sensors::instance().getThetaSensors()->getMeasurements()->getValidCount();
	ThetaMsmnt::MeasurementArray *measurementArray =
			Sensors::instance().getThetaSensors()->getMeasurementArray();

	// we send all found sensors, even if they timed out
	for (uint8_t i = 0; i < sensorCount; i++) {
		osSemaphoreAcquire(localMsmntSemHandle, 0);
		MeasurementType payload = radioMessage->getPayload();
		payload = measurementArray->at(i); // FIXME to payload data is assigned twice
		osSemaphoreRelease(localMsmntSemHandle);

		radioMessage->prepare(MsgClass::MEASUREMENT);
		// TODO send radioMessage via huart2
		// RASPY_UART
		// _nRF24L01_Basis.transmitPacket(radioMessage->getMessagePtr(),
		// 		RADIO_MESSAGE_LEN);
		// while (_nRF24L01_Basis.getLastTxResult()
		// 		== NRF24L01::nRF24_TX_IS_ONGOING) {
		// 	OsHelpers::delay(1);
		// }


	}*/



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
