/*
 * CrcSocket.h
 *
 *  Created on: 03.10.2020
 *      Author: harald
 */

#ifndef SOCKETS_CRCSOCKET_H_
#define SOCKETS_CRCSOCKET_H_

#include <Config/Types/AppTypes.h>
#include <stm32f1xx.h>

extern CRC_HandleTypeDef hcrc;

constexpr uint8_t UINT32_BYTELEN = UINT32_WIDTH / 8;

class CrcSocket {
public:
	CrcSocket() {
	}
	;
	virtual ~CrcSocket() {
	}
	;
	/**
	 * Calls the CRC-Engine and returns a 8-bit CRC, which is
	 * simply the truncation of the 32-bit result.
	 * !! dataLen MUST BE DIVIDABLE BY 4 !!
	 * Params:
	 * data: pointer to array
	 * dataLen: size of the Array in bytes
	 */
	static uint8_t calcChksum(uint8_t *data, uint8_t dataByteLen) {
		if ((dataByteLen % 4) != 0) {
			return 0;
		}
		uint32_t crc = HAL_CRC_Calculate(&hcrc,
				reinterpret_cast<uint32_t*>(data), dataByteLen / 4);
		return static_cast<uint8_t>(crc);
	}
	;
	/**
	 * Calculates the size in uint32 multiples. Takes in the size in Bytes and returns
	 * how many uint32 values these are, if rounded up.
	 *  i.E. char[3] = 3 bytes = 1 uint32; char[5] = 5 bytes = 2 uint32;
	 */
	static uint32_t calcUint32Len(uint32_t sizeInBytes) {
		uint32_t len32 = (sizeInBytes + 3) / 4;
		return len32;
	}
	;
	/**
	 * Takes in data of any size, buffers it in a uint32-array and pads the remainig
	 * bytes. The resulting array can directly be passed to the CRC-engine.
	 */
	static uint32_t calcBufferedChkSum32(uint8_t *data, uint32_t dataByteLen) {
		uint32_t uint32len = calcUint32Len(dataByteLen);
		uint32_t data32[uint32len];
		uint8_t *uint8Data = reinterpret_cast<uint8_t*>(data32);

		// forget memset, it works bytewise
		for (uint_fast8_t i = 0; i < uint32len; i++) {
			data32[i] = 0;
		}
		for (uint_fast8_t i = 0; i < dataByteLen; i++) {
			uint8Data[i] = data[i];
		}
		return CrcSocket::calcChksum32(data32, uint32len);
	}
	;
	/**
	 * MainFunction, passes the data to the crc-engine
	 */
	static uint32_t calcChksum32(uint32_t *data, uint32_t dataLen32) {
		uint32_t crc = HAL_CRC_Calculate(&hcrc, data, dataLen32);
		return crc;
	}
	;

};

#endif /* SOCKETS_CRCSOCKET_H_ */
