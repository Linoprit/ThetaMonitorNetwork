/*
 * CalcCrc.cpp
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#include "CalcCrc.h"
#include <iostream>

CalcCrc::CalcCrc() :
		actCrc { 0 } {
}

void CalcCrc::resetCrc(void) {
	actCrc = DEFAULT_CRC_INITVALUE;
}

uint32_t CalcCrc::calcCrcUpdate(uint32_t data) {
	uint8_t i;
	actCrc = actCrc ^ data;
	for (i = 0; i < 32; i++) {
		if (actCrc & 0x80000000)
			actCrc = (actCrc << 1) ^ DEFAULT_CRC32_POLY;
		else
			actCrc <<= 1;
	}
	return actCrc;
}

uint32_t CalcCrc::crcAccumulate(uint32_t pBuffer[], uint32_t BufferLength) {
	uint32_t i;
	for (i = 0U; i < BufferLength; i++) {
		calcCrcUpdate(pBuffer[i]);
	}
	return actCrc;
}

uint32_t CalcCrc::crcCalculate(vector<uint32_t> buffer) {
	resetCrc();
	for (uint32_t item : buffer) {
		calcCrcUpdate(item);
	}
	return actCrc;
}

uint32_t CalcCrc::crcCalculate(uint32_t pBuffer[], uint32_t BufferLength) {
	resetCrc();
	for (uint32_t i = 0U; i < BufferLength; i++) {
		calcCrcUpdate(pBuffer[i]);
	}
	return actCrc;
}
