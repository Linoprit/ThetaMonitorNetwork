#include "stm32f1xx_hal.h"
#include <stm32f1xx_hal_crc.h>
#include <stdio.h>

uint32_t actCrc; // current crc-value
void resetCrc(void) {
	actCrc = DEFAULT_CRC_INITVALUE;
}

uint32_t calcCrcUpdate(uint32_t data) {
	uint8_t i;
	actCrc = actCrc ^ data;
	for (i = 0; i < 32; i++) {
		if (actCrc &  0x80000000)
			actCrc = (actCrc << 1) ^ DEFAULT_CRC32_POLY;
		else
			actCrc <<= 1;
	}
	return actCrc;
}

uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[],
		uint32_t BufferLength) {
	uint32_t i;

	for (i = 0U; i < BufferLength; i++) {
		calcCrcUpdate(pBuffer[i]);
	}
	return actCrc;
}

uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[],
		uint32_t BufferLength) {

	resetCrc();
	for (uint32_t i=0U; i < BufferLength; i++) {
		calcCrcUpdate(pBuffer[i]);
	}
	return actCrc;
}

