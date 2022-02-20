/*
 * nRF24L01_Basis.cpp
 *
 *  Created on: 05.11.2018
 *      Author: harald
 */

#include <Application/RadioLink/NRF24L01_Basis.h>
#include <Application/Sensors/Sensors.h>
#include <System/OsHelpers.h>
#include <Config/config.h>
#include <main.h>
#include <System/serialPrintf.h>
#include <string>

using namespace snsrs;

NRF24L01_Basis::NRF24L01_Basis() :
		_lostPkgCount { 0 }, _retransCount { 0 }, _lastTxResult {
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
		OsHelpers::delay(10);
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
		_nRF24.EnableAA(0);
		_nRF24.EnableAA(1);
		_nRF24.EnableAA(2);
		_nRF24.EnableAA(3);
		_nRF24.EnableAA(4);
		_nRF24.EnableAA(5);

	} else if (stationType == SensorIdTable::SLAVE_01) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR1[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR1[0]);
		_nRF24.SetRXPipe(nRF24_PIPE1, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.EnableAA(1);
	} else if (stationType == SensorIdTable::SLAVE_02) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR2[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR2[0]);
		_nRF24.SetRXPipe(nRF24_PIPE2, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.EnableAA(2);
	} else if (stationType == SensorIdTable::SLAVE_03) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR3[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR3[0]);
		_nRF24.SetRXPipe(nRF24_PIPE3, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.EnableAA(3);
	} else if (stationType == SensorIdTable::SLAVE_04) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR4[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR4[0]);
		_nRF24.SetRXPipe(nRF24_PIPE4, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.EnableAA(4);
	} else if (stationType == SensorIdTable::SLAVE_05) {
		_nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR5[0]); // program TX address
		_nRF24.SetAddr(nRF24_PIPE0, &nRF24_RX_ADDR5[0]);
		_nRF24.SetRXPipe(nRF24_PIPE5, nRF24_AA, RADIO_MESSAGE_LEN);
		_nRF24.EnableAA(5);
	}

	_nRF24.SetAutoRetr(nRF24_ARD_500us, 8); // auto-retransmission
	_nRF24.SetTXPower(nRF24_TXPWR_18dBm); // TODO power up, when deploy
	_nRF24.ClearIRQFlags();
	_nRF24.SetPowerMode(nRF24_PWR_UP); // Wake the transceiver
	_nRF24.SetOperationalMode(nRF24_MODE_RX);
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

void NRF24L01_Basis::transmitPacket(uint8_t *pBuf, uint8_t length) {
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
}

void NRF24L01_Basis::addStatistics(uint8_t lostPkgCount, uint8_t retransCount) {
	this->_lostPkgCount += lostPkgCount;
	this->_retransCount += retransCount;
}

// will be called after transmission is done
void NRF24L01_Basis::IrqPinRxCallback(void) {
	// we are in TX-IRQ, because we started a transmission
	if (_lastTxResult == NRF24L01::nRF24_TX_IS_ONGOING) {
		uint8_t status = _nRF24.GetStatus();
		_nRF24.CE_L();	// Deassert the CE pin (Standby-II --> Standby-I)
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
			_nRF24.ReadPayload(nRF24_payload, &payload_length);
			_nRF24.ClearIRQFlags();

			// we make sure, always to put a whole message into the queue,
			// so we never get problems with getting out of sync, with
			// data fragments.
			osSemaphoreAcquire(nRF_RxBuffSemHandle, 10);
			if (_rxBuffer.size() >= static_cast<int16_t>(nRF24_RX_QUEUE_SIZE)) {
				_rxBufferOverflows++;
			} else {
				for (uint8_t i = 0; i < RADIO_MESSAGE_LEN; i++) {
					_rxBuffer.enqueue(nRF24_payload[i]);
				}
			}
			osSemaphoreRelease(nRF_RxBuffSemHandle);
		} // measured: 190us
	}
}
