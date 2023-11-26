/*
 * nRF24L01_Basis.h
 *
 *  Created on: 05.11.2018
 *      Author: harald
 */

#ifndef RADIOLINK_NRF24L01_BASIS_H_
#define RADIOLINK_NRF24L01_BASIS_H_

#include <sys/_stdint.h>

// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#include "stdbool.h"
EXTERNC bool stationType_isMaster(void);

#undef EXTERNC
#ifdef __cplusplus

#include <vector>
#include <Libraries/SimpleQueue.h>
#include <Config/callbacks.h>
#include <System/Devices/nRF24L01/NRF24L01.h>
#include <Middleware/NRF24L01_Radio/NRF24L01_Basis.h>
#include <Libraries/SimpleQueue.h>
#include <Config/config.h>
#include <Application/Radio/RadioMessage.h>
#include <string>

static constexpr uint8_t nRF24_CHANNEL = 120;
static constexpr int32_t nRF24_WAIT_TIMEOUT = (int32_t) 0x0000FFFF;
static constexpr uint8_t nRF_AUTO_RETRY = 10;

// TX addresses
static constexpr uint8_t nRF24_TX_ADDR1[] = { 'E', 'S', 'B', 'X', 'Z' }; // Slave 1
static constexpr uint8_t nRF24_TX_ADDR2[] = { 'E', 'S', 'B', 'X', 'Y' }; // Slave 2
static constexpr uint8_t nRF24_TX_ADDR3[] = { 'E', 'S', 'B', 'X', 'X' }; // Slave 3
static constexpr uint8_t nRF24_TX_ADDR4[] = { 'E', 'S', 'B', 'X', 'W' }; // Slave 4
static constexpr uint8_t nRF24_TX_ADDR5[] = { 'E', 'S', 'B', 'X', 'V' }; // Slave 5

// RX addresses
static constexpr uint8_t nRF24_RX_ADDR1[] = { 'E', 'S', 'B', 'X', 'Z' }; // Slave 1
static constexpr uint8_t nRF24_RX_ADDR2[1] = { 'Y' };                 // Slave 2
static constexpr uint8_t nRF24_RX_ADDR3[1] = { 'X' };                 // Slave 3
static constexpr uint8_t nRF24_RX_ADDR4[1] = { 'W' };                 // Slave 4
static constexpr uint8_t nRF24_RX_ADDR5[1] = { 'V' };                 // Slave 5

class NRF24L01_Basis {
public:
	//typedef SimpleQueue<uint8_t, nRF24_RX_QUEUE_SIZE> RxBufferQueue;
	enum nRFState {
		none, checkOk, initDone
	};

	NRF24L01_Basis();
	virtual ~NRF24L01_Basis() {
	}

	void init();
	// Blocks, until transmission is done
	NRF24L01::nRF24_TXResult transmitPacket(uint8_t *pBuf, uint8_t length = 0);
	// needs the Irq-callback tied to nRF-IRQ-Pin
	void transmitPacket_IRQ(uint8_t *pBuf, uint8_t length = 0);
	void IrqPinRxCallback(void);

	std::string getNRFStateStr(void);
	nRFState getNRFState(void) {
		return _nRFState;
	}
	uint8_t get_lostPkgCount(void) {
		return _nrfLostPkgCount;
	}
	uint8_t get_retransCount(void) {
		return _nrfRetransCount;
	}
	uint8_t get_rxBufferOverflows(void) {
		return _rxBufferOverflows;
	}
	void resetStatistics(void) {
		_nrfLostPkgCount = 0;
		_nrfRetransCount = 0;
		_rxBufferOverflows = 0;
	}
	void addStatistics(uint8_t lostPkgCount, uint8_t retransCount);

	NRF24L01::nRF24_TXResult getLastTxResult(void) {
		return _lastTxResult;
	}

	// only for debugging
	NRF24L01* getNrf24(void){
		return &_nRF24;
	}

	// To be called cyclically, if data was sent by transmitPacket_IRQ.
	// Returns true, if transmission is still in progress,
	// and checks for transmission time-out.
	bool isTxOngoing(void);

	// returns true, if last finished transmission was successful
	bool lastTxWasSuccess(void);

private:
	NRF24L01 _nRF24;
	uint8_t _nrfLostPkgCount;
	uint8_t _nrfRetransCount;
	uint8_t _rxBufferOverflows;
	NRF24L01::nRF24_TXResult _lastTxResult;
	uint32_t _tickTransmissionStarted;  // when start transmission, current tick saved here
	nRFState _nRFState;
};

#endif // C interface
#endif /* RADIOLINK_NRF24L01_BASIS_H_ */
