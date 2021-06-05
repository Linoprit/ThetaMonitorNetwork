/*
 * Unit_at24c256.cpp
 *
 *  Created on: 03.03.2021
 *      Author: harald
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stm32f1xx.h>
#include <EepromEmulation.h>
#include <Devices/Eeprom_at24c256/AT24Cxxx.h>


// for emulation of eeprom chip at24c256 connected to i2c1
I2C_HandleTypeDef hi2c1;

using ::testing::Expectation;

class Eeprom_at24c256_Fix: public ::testing::Test {
public:

	// void SetUp() {};
	// void TearDown() {};

	void fillEepromWith(uint8_t value) {
		uint8_t *eepromBuffer = EepromEmulation::get_buffer();
		memset((void*) eepromBuffer, value, EepromEmulation::MAX_ADDRESS + 1U);
	}
};

TEST_F(Eeprom_at24c256_Fix, eraseFunction) {
	EepromEmulation::init();
	fillEepromWith(0xAA);

	const uint16_t buffLen = 300;
	const uint16_t startAddr = 500U;
	uint8_t rxData[buffLen];

	AT24Cxxx::erase(startAddr, buffLen);
	//EepromEmulation::dump(448, 831);
	AT24Cxxx::read(startAddr, rxData, buffLen);
	for (uint16_t i = 0; i < buffLen; i++) {
		if (rxData[i] != 0xFF) {
			FAIL() << "eraseFunction at address " << i;
		}
	}
}

TEST_F(Eeprom_at24c256_Fix, devideReadWrite) {
	EepromEmulation::init();

	const uint8_t buffLen = 92;
	const uint16_t startAddr = 50U;
	uint8_t txData[buffLen];
	uint8_t rxData[buffLen];
	for (uint16_t i = 0; i < buffLen; i++) {
		txData[i] = (i & 0x00FF);
	}
	AT24Cxxx::write(startAddr, txData, buffLen);
	// EepromEmulation::dump(0, 255);

	 AT24Cxxx::read(startAddr, rxData, buffLen);
	for (uint16_t i = 0; i < buffLen; i++) {
		if (rxData[i] != (i & 0x00FF)) {
			FAIL() << "devideReadWrite read/write at address " << i;
		}
	}
	uint8_t finalByte = eepromEmulation_readByte(50U + 92U);
	EXPECT_EQ(finalByte, 255U);
}

TEST_F(Eeprom_at24c256_Fix, testEmulation) {
	EepromEmulation::init();
	EepromEmulation::setPageWrite(false);

	// Initial '0xFF' in the memory?
	for (uint16_t i = 0; i < EepromEmulation::MAX_ADDRESS + 1; i++) {
		uint8_t result = EepromEmulation::read(i);
		if (result != 0xFF) {
			FAIL() << "basisTest init at address " << i;
		}
	}

	// write / read test
	for (uint16_t i = 0; i < EepromEmulation::MAX_ADDRESS + 1; i++) {
		EepromEmulation::write(i, i);
	}
	for (uint16_t i = 0; i < EepromEmulation::MAX_ADDRESS + 1; i++) {
		uint8_t result = EepromEmulation::read(i);
		if (result != (i & 0x00FF)) {
			FAIL() << "basisTest read/Write at address " << i;
		}
	}

	EepromEmulation::erase();
	for (uint16_t i = 0; i < EepromEmulation::MAX_ADDRESS + 1; i++) {
		uint8_t result = EepromEmulation::read(i);
		if (result != 0xFF) {
			FAIL() << "basisTest erase at address " << i;
		}
	}

	fillEepromWith(0xAA); // make sure, at the last address is no '0xFF'
	uint8_t txData[EepromEmulation::MAX_ADDRESS];
	uint8_t rxData[EepromEmulation::MAX_ADDRESS];
	for (uint16_t i = 0; i < EepromEmulation::MAX_ADDRESS + 1; i++) {
		txData[i] = (i & 0x00FF);
	}
	EepromEmulation::write(0, txData, EepromEmulation::MAX_ADDRESS + 1);
	EepromEmulation::read(0, rxData, EepromEmulation::MAX_ADDRESS + 1);
	for (uint16_t i = 0; i < EepromEmulation::MAX_ADDRESS + 1; i++) {
		if (rxData[i] != (i & 0x00FF)) {
			FAIL() << "basisTest sequential read/write at address " << i
					<< "\n   expected: " << i << " got: " << rxData[i];
		}
	}

	fillEepromWith(0xAA); // make sure, at the last address is no '0xFF'
	memset((void*) &rxData, 0xBB, EepromEmulation::MAX_ADDRESS + 1U);
	EepromEmulation::write(0, txData, 10);
	EepromEmulation::read(0, rxData, 10);
	for (uint16_t i = 0; i < 10; i++) {
		if (rxData[i] != (i & 0x00FF)) {
			FAIL() << "basisTest limited seq read/write at address " << i
					<< "\n   expected: " << i << " got: " << rxData[i];
		}
	}
	//EepromEmulation::dump(); // if needed, show the eeprom-contents
}

TEST_F(Eeprom_at24c256_Fix, emulationPaging) {
	EepromEmulation::init();

	// roll-over-test writing out of page limit
	fillEepromWith(0xAA);
	const uint8_t buffLen = 20;
	const uint16_t startAddr = 50U;
	uint8_t txData[buffLen];
	for (uint16_t i = 0; i < buffLen + 1; i++) {
		txData[i] = (i & 0x00FF);
	}
	EepromEmulation::write(startAddr, txData, buffLen);

	// @formatter:off
	const uint8_t refData[] {
	 14U,  15U,  16U,  17U,   18U,  19U, 170U, 170U,   170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U,
	170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U,   170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U,
	170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U,   170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U,
	170U, 170U,   0U,   1U,    2U,   3U,   4U,   5U,     6U,   7U,   8U,   9U,   10U,  11U,  12U,  13U,
	170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U,   170U, 170U, 170U, 170U,  170U, 170U, 170U, 170U
    };
	// @formatter:on
	const uint16_t refDataLen = sizeof(refData);
	uint8_t rxData[refDataLen];
	EepromEmulation::read(0, rxData, refDataLen);
	for (uint16_t i = 0; i < refDataLen; i++) {
		if (rxData[i] != refData[i]) {
			FAIL() << "emulationPaging rollOver at address " << i;
		}
	}
}

/*
 * This is not really a test of the emulation. Here, we test, if the algorithm,
 * with which we will write to the AT24xxx works.
 */
TEST_F(Eeprom_at24c256_Fix, doPagedWriting) {
	EepromEmulation::init();
	fillEepromWith(0xAA);
	const uint8_t buffLen = 92;
	const uint16_t startAddr = 50U;
	uint8_t txData[buffLen];
	uint8_t rxData[buffLen];
	for (uint16_t i = 0; i < buffLen + 1; i++) {
		txData[i] = (i & 0x00FF);
	}

	uint16_t startPage = EepromEmulation::pageAtAddress(startAddr);
	uint16_t endPage = EepromEmulation::pageAtAddress(startAddr + buffLen);
	EXPECT_EQ(startPage, 0U);
	EXPECT_EQ(endPage, 2U);

	uint16_t tempStartAddr = startAddr;
	const uint16_t endAddr = startAddr + buffLen;
	for (uint16_t page = startPage; page <= endPage; page++) {
		uint16_t tempEndAddr = std::min(endAddr,
				EepromEmulation::upperAddressFromPageNr(page));
		uint16_t tmpLen = tempEndAddr - tempStartAddr + 1;
		uint16_t offset = tempStartAddr - startAddr;

		EepromEmulation::write(tempStartAddr, &txData[offset], tmpLen);

		// watch the single steps
		// printf("\nStep %i\n", page);
		// EepromEmulation::dump(16, 191);

		tempStartAddr = std::min(endAddr,
				EepromEmulation::lowerAddressFromPageNr(page + 1));
	}

	EepromEmulation::read(startAddr, rxData, buffLen);
	for (uint16_t i = 0; i < buffLen; i++) {
		if (rxData[i] != (i & 0x00FF)) {
			FAIL() << "doPagedWriting read/write at address " << i;
		}
	}
}
