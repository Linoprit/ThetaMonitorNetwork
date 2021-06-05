/*
 * CrcSocket.cpp
 *
 *  Created on: 03.10.2020
 *      Author: harald
 */

#include <Sockets/CrcSocket.h>
#include <System/Error_Handler.h>


/**
 * Calls the CRC-Engine and returns a 8-bit CRC, which is
 * simply the truncation of the 32-bit result.
 * !! dataLen MUST BE DIVIDABLE BY 4 !!
 * Params:
 * data: pointer to array
 * dataLen: size of the Array in bytes
 */
uint8_t CrcSocket::calc_chksum(uint8_t* data, uint8_t dataLen) {
	if((dataLen % 4) != 0) {
		 return 0;
	}

	uint32_t crc =
			HAL_CRC_Calculate(
					&hcrc,
					reinterpret_cast<uint32_t*>(data),
					dataLen/4);

	return static_cast<uint8_t>(crc);
}

uint32_t CrcSocket::calc_chksum32(uint32_t* data, uint32_t dataLen32) {
	if(dataLen32 == 0) {
		return 0;
	}
	uint32_t crc = HAL_CRC_Calculate( &hcrc, data, dataLen32);
	return crc;
}

/*uint8_t CrcSocket::calc_chksum(uint8_t* data, uint8_t dataLen) {
  uint8_t i, crc = 0;

  // we must exclude the chk-sum byte at the end!
  for (i=0; i < (dataLen-1); i++) {
      crc = _crc_ibutton_update(crc, *data);
      data++;
    }
  return crc;
}

uint8_t CrcSocket::_crc_ibutton_update(uint8_t crc, uint8_t data) {
  uint8_t i;

  crc = crc ^ data;
  for (i = 0; i < 8; i++)  {
      if (crc & 0x01)
        crc = (crc >> 1) ^ 0x8C;
      else
        crc >>= 1;
    }

  return crc;
  }
 */
