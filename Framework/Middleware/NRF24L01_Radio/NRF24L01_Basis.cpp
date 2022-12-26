/*
 * nRF24L01_Basis.cpp
 *
 *  Created on: 05.11.2018
 *      Author: harald
 */

#include "NRF24L01_Basis.h"
#include <Application/Sensors/Sensors.h>
#include <System/OsHelpers.h>
#include <Config/config.h>
#include <main.h>
#include <System/serialPrintf.h>
#include <string>

using namespace snsrs;

NRF24L01_Basis::NRF24L01_Basis() :
		_nrfLostPkgCount { 0 }, _nrfRetransCount { 0 }, _lastTxResult {
				NRF24L01::nRF24_NOP }, _nRFState { none } {
}

void NRF24L01_Basis::init() {
	_nRF24.CE_L(); // RX/TX disabled

	SensorIdTable::StationType stationType =
			Sensors::instance().getNonVolatileData()->getStationType();
	tx_printf("Station Type is %u\n", static_cast<uint8_t>(stationType));

	// Configure the nRF24L01+
	tx_printf("nRF24L01+ checking... ");
	while (!_nRF24.Check()) {
		OsHelpers::delay(100);
	}
	_nRFState = checkOk;
	tx_printf(" OK\n");

	_nRF24.Init(); // Initialize the nRF24L01 to its default state

	_nRF24.SetRFChannel(nRF24_CHANNEL);
	_nRF24.SetDataRate(nRF24_DR_250kbps); //  2Mbps);
	_nRF24.SetCRCScheme(nRF24_CRC_2byte);
	_nRF24.SetAddrWidth(5);

	const uint8_t nRF24_AA = nRF24_AA_ON; // advanced shockburst
	if (stationType == SensorIdTable::MASTER) {
		_nRF24.SetAddr(nRF24_PIPE1, &nRF24_RX_ADDR1[0]);
		_nRF24.SetRXPipe(nRF24_PIPE1, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.SetAddr(nRF24_PIPE2, &nRF24_RX_ADDR2[0]);
		_nRF24.SetRXPipe(nRF24_PIPE2, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.SetAddr(nRF24_PIPE3, &nRF24_RX_ADDR3[0]);
		_nRF24.SetRXPipe(nRF24_PIPE3, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.SetAddr(nRF24_PIPE4, &nRF24_RX_ADDR4[0]);
		_nRF24.SetRXPipe(nRF24_PIPE4, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.SetAddr(nRF24_PIPE5, &nRF24_RX_ADDR5[0]);
		_nRF24.SetRXPipe(nRF24_PIPE5, nRF24_AA, RADIO_MESSAGE_LEN);

		_nRF24.SetRXPipe(nRF24_PIPE0, nRF24_AA, RADIO_MESSAGE_LEN);

	} else if (stationType == SensorIdTable::SLAVE_01) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR1[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR1[0]);
		_nRF24.SetRXPipe(nRF24_PIPE1, nRF24_AA, RADIO_MESSAGE_LEN);
	} else if (stationType == SensorIdTable::SLAVE_02) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR2[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR2[0]);
		_nRF24.SetRXPipe(nRF24_PIPE2, nRF24_AA, RADIO_MESSAGE_LEN);
	} else if (stationType == SensorIdTable::SLAVE_03) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR3[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR3[0]);
		_nRF24.SetRXPipe(nRF24_PIPE3, nRF24_AA, RADIO_MESSAGE_LEN);
	} else if (stationType == SensorIdTable::SLAVE_04) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR4[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR4[0]);
		_nRF24.SetRXPipe(nRF24_PIPE4, nRF24_AA, RADIO_MESSAGE_LEN);
	} else if (stationType == SensorIdTable::SLAVE_05) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR5[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR5[0]);
		_nRF24.SetRXPipe(nRF24_PIPE5, nRF24_AA, RADIO_MESSAGE_LEN);
	}

	_nRF24.EnableAA(0xFF);  // for all pipes
	_nRF24.SetAutoRetr(nRF24_ARD_2500us, 10); // auto-retransmission
	_nRF24.SetTXPower(nRF24_TXPWR_18dBm); // TODO power up, when deploy
	_nRF24.SetOperationalMode(nRF24_MODE_RX);
	_nRF24.ClearIRQFlags();
	_nRF24.SetPowerMode(nRF24_PWR_UP); // Wake the transceiver
	// Enable dynamic payload
	_nRF24.SetDynamicPayloadLength(nRF24_DPL_ON);
	_nRF24.SetPayloadWithAck(1);

	_nRF24.CE_H();

	_nRFState = initDone;
	tx_printf("Radio init done.\n");
}

std::string NRF24L01_Basis::getNRFStateStr(void) {
	if (_nRFState == none) {
		return std::string("none");
	} else if (_nRFState == checkOk) {
		return std::string("checkOk");
	} else if (_nRFState == initDone) {
		return std::string("initDone");
	}
	return std::string("none");
}

NRF24L01::nRF24_TXResult NRF24L01_Basis::transmitPacket(uint8_t *pBuf, uint8_t length) {
	uint8_t deviceStatus;
	volatile uint32_t wait = nRF24_WAIT_TIMEOUT;

	// in Case of no success, we have to flush the fifos
	if (_lastTxResult != NRF24L01::nRF24_TX_SUCCESS) {
		_nRF24.FlushRX();
		_nRF24.FlushTX();
	}
	_nRF24.ClearIRQFlags(); // Clear any pending interrupt flags
	_nRF24.CE_L(); // Deassert the CE pin (in case if it still high)

	_nRF24.SetOperationalMode(nRF24_MODE_TX);
	_nRF24.WritePayload(pBuf, length); // Transfer data to the TX FIFO
	_nRF24.CE_H(); // Start transmission. CE pin must be held at least 10us

	do {
		deviceStatus = _nRF24.GetStatus();
		if (deviceStatus & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT )) {
			break;
		}
		OsHelpers::delay(10);
	} while (wait--);

	_nRF24.CE_L();	// De-assert the CE pin (Standby-II --> Standby-I)
	_nRF24.ClearIRQFlags();
	_nRF24.SetOperationalMode(nRF24_MODE_RX);

	// statistics
	uint8_t otx = _nRF24.GetRetransmitCounters();
	uint8_t otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT ) >> 4; // packets lost counter
	uint8_t otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT ); // auto retransmissions counter
	addStatistics(otx_plos_cnt, otx_arc_cnt);

	if (!wait) {
		_lastTxResult = NRF24L01::nRF24_TX_TIMEOUT;
	} else if (deviceStatus & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		_lastTxResult = NRF24L01::nRF24_TX_MAXRT;
	} else if (deviceStatus & nRF24_FLAG_TX_DS) {
		_lastTxResult = NRF24L01::nRF24_TX_SUCCESS;
	} else {
		// Some banana happens, a payload remains in the TX FIFO, flush it
		_nRF24.FlushTX();
		_lastTxResult = NRF24L01::nRF24_TX_ERROR;
	}
	return _lastTxResult;
}

void NRF24L01_Basis::transmitPacket_IRQ(uint8_t *pBuf, uint8_t length) {
	// in Case of no success, we have to flush the fifos
	if (_lastTxResult != NRF24L01::nRF24_TX_SUCCESS) {
		_nRF24.FlushRX();
		_nRF24.FlushTX();
	}
	_nRF24.ClearIRQFlags(); // Clear any pending interrupt flags
	_nRF24.CE_L(); // Deassert the CE pin (in case if it still high)

	_nRF24.SetOperationalMode(nRF24_MODE_TX);
	_nRF24.WritePayload(pBuf, length); // Transfer data to the TX FIFO

	_nRF24.CE_H(); // Start transmission. CE pin must be held at least 10us
	_lastTxResult = NRF24L01::nRF24_TX_IS_ONGOING;
	_tickTransmissionStarted = OsHelpers::get_tick();
	// CAUTION: if the IRQ never occurs, _lastTxResult keeps its state forever
}

void NRF24L01_Basis::addStatistics(uint8_t lostPkgCount, uint8_t retransCount) {
	if(_nrfLostPkgCount <= 255){
		_nrfLostPkgCount += lostPkgCount;
	}
	if(_nrfRetransCount <= 255){
		_nrfRetransCount += retransCount;
	}
	_nRF24.ResetPLOS();
}

// will be called after transmission is done
void NRF24L01_Basis::IrqPinRxCallback(void) {
	// we are in TX-IRQ, because we started a transmission
	if ((_lastTxResult == NRF24L01::nRF24_TX_IS_ONGOING) ||
			(_lastTxResult == NRF24L01::nRF24_TX_TIMEOUT)) {
		uint8_t status = _nRF24.GetStatus();
		_nRF24.CE_L();	// De-assert the CE pin (Standby-II --> Standby-I)
		_nRF24.ClearIRQFlags();
		_nRF24.SetOperationalMode(nRF24_MODE_RX);

		// statistics
		uint8_t otx = _nRF24.GetRetransmitCounters();
		uint8_t otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT ) >> 4; // packets lost counter
		uint8_t otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT ); // auto retransmissions counter
		addStatistics(otx_plos_cnt, otx_arc_cnt);

		if (status & nRF24_FLAG_MAX_RT) {
			// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
			_lastTxResult = NRF24L01::nRF24_TX_MAXRT;
		} else if (status & nRF24_FLAG_TX_DS) {
			_lastTxResult = NRF24L01::nRF24_TX_SUCCESS;
		}
	} else { // not in tx-condition => rx-interrupt
		uint8_t payload_length;
		uint8_t nRF24_payload[RADIO_MESSAGE_LEN];

		if (_nRF24.GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
			_nRF24.ReadPayloadDpl(nRF24_payload, &payload_length);
			_nRF24.ClearIRQFlags();

			// we make sure, always to put a whole message into the queue,
			// so we never get problems with getting out of sync, with
			// data fragments.
			osStatus_t queue_status =
					osMessageQueuePut(remoteDataQueueHandle, nRF24_payload, 5, 0);
			if(queue_status != osOK){
				_rxBufferOverflows++;
			}
		}
	}
}

bool NRF24L01_Basis::isTxOngoing(void) {
	NRF24L01::nRF24_TXResult result = getLastTxResult();
	uint32_t tickDiff = TX_ONGOING_MAX_TICKS;

	if(OsHelpers::get_tick() > _tickTransmissionStarted){ // tick-overflow?
		tickDiff = OsHelpers::get_tick() - _tickTransmissionStarted;
	}

	if (result != NRF24L01::nRF24_TX_IS_ONGOING) {
		return false;
	}
	if (tickDiff > TX_ONGOING_MAX_TICKS) {
		_lastTxResult = NRF24L01::nRF24_TX_TIMEOUT;
		// TODO do nrf-init?
		_nrfLostPkgCount++;
		return false;
	}
	return true;
}

bool NRF24L01_Basis::lastTxWasSuccess(void) {
	NRF24L01::nRF24_TXResult result = getLastTxResult();
	if (result != NRF24L01::nRF24_TX_SUCCESS) {
		if ((result == NRF24L01::nRF24_TX_MAXRT)
				|| (result == NRF24L01::nRF24_TX_TIMEOUT)
				|| (result == NRF24L01::nRF24_TX_ERROR)) {
			return false;
		}
	}
	return true;
}

