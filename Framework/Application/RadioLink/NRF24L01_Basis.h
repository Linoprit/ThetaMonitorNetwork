/*
 * nRF24L01_Basis.h
 *
 *  Created on: 05.11.2018
 *      Author: harald
 */

#ifndef RADIOLINK_NRF24L01_BASIS_H_
#define RADIOLINK_NRF24L01_BASIS_H_

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
#include <Devices/nRF24L01/NRF24L01.h>
#include <Libraries/SimpleQueue.h>
//#include "../Application/ThetaSensors/ID_Table.h"
#include "Messages.h"

static constexpr uint8_t nRF24_CHANNEL = 115;
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

static constexpr uint8_t RX_BUFFER_SIZE = 127;

class NRF24L01_Basis {
public:
	NRF24L01_Basis();
	virtual ~NRF24L01_Basis() {
	}
	;

	void init();
	NRF24L01::nRF24_TXResult transmitPacket(uint8_t *pBuf, uint8_t length = 0);

	// NRF24L01* get_nRF24(void) {
	// 	return nRF24;
	// }
	// ;
	uint16_t get_lostPkgCount(void) {
		return lostPkgCount;
	}
	;
	uint16_t get_retransCount(void) {
		return retransCount;
	}
	;
	void reset_stats(void) {
		lostPkgCount = 0;
		retransCount = 0;
	}
	;
	void add_stats(uint8_t lostPkgCount, uint8_t retransCount);

	void IRQ_Pin_callback(void);

private:
	NRF24L01 nRF24;
	uint16_t lostPkgCount;
	uint16_t retransCount;
	SimpleQueue<uint8_t, RX_BUFFER_SIZE> rxBuffer;

};

#endif // C interface
#endif /* RADIOLINK_NRF24L01_BASIS_H_ */
