/*
 * CrcSocket.cpp
 *
 *  Created on: 03.10.2020
 *      Author: harald
 */

#include <Sockets/CrcSocket.h>
#include <stm32f1xx.h>

/**
 * Calls the CRC-Engine and returns a 8-bit CRC, which is
 * simply the truncation of the 32-bit result.
 * !! dataLen MUST BE DIVIDABLE BY 4 !!
 * Params:
 * data: pointer to array
 * dataLen: size of the Array in bytes
 */
uint8_t CrcSocket::calc_chksum(uint8_t *data, uint8_t dataLen) {
	if ((dataLen % 4) != 0) {
		return 0;
	}

	uint32_t crc = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(data),
			dataLen / 4);

	return static_cast<uint8_t>(crc);
}

uint32_t CrcSocket::calc_chksum32(uint32_t *data, uint32_t dataLen32) {
	uint32_t crc = HAL_CRC_Calculate(&hcrc, data, dataLen32);
	return crc;
}

/**
 * Calculates the size in uint32 multiples. Takes in the size in Bytes and returns
 * how many uint32 values these are, if rounded up.
 *  i.E. char[3] = 3 bytes = 1 uint32; char[5] = 5 bytes = 2 uint32;
 */
uint32_t CrcSocket::calcUint32Len(uint32_t sizeInBytes) {
	uint32_t len32 = (sizeInBytes + 3) / 4;
	return len32;
}


