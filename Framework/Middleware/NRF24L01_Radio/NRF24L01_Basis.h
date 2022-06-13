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
#include <Application/RadioLink/RadioMessage.h>
#include <string>

static constexpr uint8_t nRF24_CHANNEL = 118;
static constexpr int32_t nRF24_WAIT_TIMEOUT = (int32_t) 0x00000FFF;
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
	typedef SimpleQueue<uint8_t, nRF24_RX_QUEUE_SIZE> RxBufferQueue;
	enum nRFState {
		none, checkOk, initDone
	};

	NRF24L01_Basis();
	virtual ~NRF24L01_Basis() {
	}

	void init();
	void transmitPacket(uint8_t *pBuf, uint8_t length = 0);
	std::string getNRFStateStr(void);
	nRFState getNRFState(void) {
		return _nRFState;
	}
	uint8_t get_lostPkgCount(void) {
		return _lostPkgCount;
	}
	uint8_t get_retransCount(void) {
		return _retransCount;
	}
	uint8_t get_rxBufferOverflows(void) {
		return _rxBufferOverflows;
	}
	void resetStatistics(void) {
		_lostPkgCount = 0;
		_retransCount = 0;
		_rxBufferOverflows = 0;
	}
	void addStatistics(uint8_t lostPkgCount, uint8_t retransCount);
	RxBufferQueue* getRxBuffer(void) {
		return &_rxBuffer;
	}
	NRF24L01::nRF24_TXResult getLastTxResult(void) {
		return _lastTxResult;
	}

	// returns true, if transmission is still in progress
	bool isTxOngoing(void){
		NRF24L01::nRF24_TXResult result = getLastTxResult();
		if (result == NRF24L01::nRF24_TX_IS_ONGOING){
			return true;
		}
		return false;
	}
	// returns true, if last finished transmission was successful
	bool lastRxWasSuccess(void){
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
	void IrqPinRxCallback(void);

private:
	NRF24L01 _nRF24;
	uint8_t _lostPkgCount;
	uint8_t _retransCount;
	uint8_t _rxBufferOverflows;
	NRF24L01::nRF24_TXResult _lastTxResult;
	nRFState _nRFState;
	RxBufferQueue _rxBuffer;
};

#endif // C interface
#endif /* RADIOLINK_NRF24L01_BASIS_H_ */
