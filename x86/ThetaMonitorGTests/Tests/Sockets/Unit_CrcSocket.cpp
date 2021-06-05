/*
 * Unit_CrcSocket.cpp
 *
 *  Created on: 14.03.2021
 *      Author: harald
 */
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stm32f1xx.h>
#include <Sockets/CrcSocket.h>
#include <System/CommandLine/ComLineConfig.h>

using ::testing::Expectation;

CRC_HandleTypeDef hcrc;

uint32_t calcUint32(uint32_t sizeInBytes) {
	// We only get strings in here, so we must ignore '\0' at the end.
	sizeInBytes = sizeInBytes - 1;
	uint32_t len32 = (sizeInBytes + 3) / 4;
	return len32;
}

TEST(CrcSocket, algorithmTest_1) {
	uint8_t data = 0xC1;
	uint8_t poly = 0xCB;
	uint8_t initialCrc = 0xFF;

	// Result in single steps
	// 0x3E 0x7C 0xF8 0x3B 0x76 0xEC 0x13 0x26 0x4C
	uint8_t i;
	uint8_t actCrc = initialCrc ^ data;
	for (i = 0; i < 8; i++) {
		if (actCrc &  0x80)
			actCrc = (actCrc << 1) ^ poly;
		else
			actCrc <<= 1;
	}
	EXPECT_EQ(actCrc, 0x4CU);
}

TEST(CrcSocket, algorithmTest_2) {
	uint32_t data = 0x11223344U;
	resetCrc();
	uint32_t crc = calcCrcUpdate(data);
	EXPECT_EQ(crc, 0xB14257CCU);
}

TEST(CrcSocket, code_1) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	uint8_t test1[] = "e2dumpjournal" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);

	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );

	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 1884395996U);
}

TEST(CrcSocket, code_2) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	char test1[] = "e2init" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);
	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );
	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 3743823511U);
}

TEST(CrcSocket, code_3) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	char test1[] = "e2dumpjournalmap" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);
	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );
	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 2362017329U);
}

TEST(CrcSocket, code_4) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	char test1[] = "e2erase" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);
	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );
	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 3704805521U);
}

TEST(CrcSocket, code_5) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	char test1[] = "e2dumpcalib" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);
	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );
	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 2821683926U);
}

TEST(CrcSocket, code_6) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	char test1[] = "e2dumpmem" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);
	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );
	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 1704472323U);
}

TEST(CrcSocket, code_7) {
	uint32_t result = 0;
	cLine::CmdBufferType cmdBuffer;
	cmdBuffer.fill('\0');

	char test1[] = "e2dumpstart" ;
	volatile uint32_t uint8Len_test1 = sizeof(test1);
	memcpy ( cmdBuffer.data(), test1, uint8Len_test1 );
	result = HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t*>(cmdBuffer.data()),
			calcUint32(sizeof(test1)));
	EXPECT_EQ(result, 302299815U);
}

