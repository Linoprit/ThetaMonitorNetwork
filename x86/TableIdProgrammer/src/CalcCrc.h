/*
 * CalcCrc.h
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#ifndef CALCCRC_H_
#define CALCCRC_H_

#include <stdint.h>
#include <vector>

using namespace std;

class CalcCrc {
public:
	CalcCrc();
	virtual ~CalcCrc() {
	}
	;

	void resetCrc(void);
	uint32_t calcCrcUpdate(uint32_t data);
	uint32_t crcAccumulate(uint32_t pBuffer[], uint32_t BufferLength);
	uint32_t crcCalculate(uint32_t pBuffer[], uint32_t BufferLength);
	uint32_t crcCalculate(vector<uint32_t> buffer);

private:
	uint32_t actCrc; // current crc-value
	static constexpr uint32_t DEFAULT_CRC32_POLY = 0x04C11DB7U;
	static constexpr uint32_t DEFAULT_CRC_INITVALUE = 0xFFFFFFFFU;
};

#endif /* CALCCRC_H_ */
