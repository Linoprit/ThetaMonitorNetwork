/*
 * CrcSocket.h
 *
 *  Created on: 03.10.2020
 *      Author: harald
 */

#ifndef SOCKETS_CRCSOCKET_H_
#define SOCKETS_CRCSOCKET_H_

#include <Types/AppTypes.h>
#include <stm32f1xx.h>

extern CRC_HandleTypeDef hcrc;


constexpr uint8_t UINT32_BYTELEN = UINT32_WIDTH / 8;

class CrcSocket {
public:
	CrcSocket() {};
	virtual ~CrcSocket() {};

	static uint32_t calcUint32Len(uint32_t sizeInBytes);
	static uint8_t calc_chksum(uint8_t* data_struct, uint8_t data_struct_len);
	static uint32_t calc_chksum32(uint32_t* data, uint32_t dataLen32);
	static uint8_t _crc32_update(uint8_t crc, uint8_t data);
};

#endif /* SOCKETS_CRCSOCKET_H_ */
