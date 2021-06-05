/*
 * InternalTests.cpp
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#include "InternalTests.h"
#include "CalcCrc.h"
#include <iostream>
#include <array>

using namespace boost::unit_test;
using namespace std;

// command line args: --log_level=test_suite --run_test=testA

bool init_function() {
	// NOP
	return true;
}

BOOST_AUTO_TEST_CASE( simple_calcCrc ) {
	CalcCrc calcCrc;
	uint32_t result = 0;

	// remember: we put 32-bit values into the crc-engine. The addresses
	// of the sensors are 8 x 8-bit values, so we can cast them into
	// two uint32 values and process them.
	uint8_t buffer_01[] { 0x28, 0xFF, 0x89, 0x0E, 0x02, 0x17, 0x03, 0x4A };
	result = calcCrc.crcCalculate(reinterpret_cast<uint32_t*>(buffer_01), 2);
	cout << "buffer_01 " << result << "\n";
	BOOST_TEST(result, 3103548024U);
}

BOOST_AUTO_TEST_CASE( array_calcCrc ) {
	CalcCrc calcCrc;
	uint32_t result = 0;
// @formatter:off
	uint8_t addressTbl[][8] =	{
			{ 0x28, 0xFF, 0x45, 0xB1, 0xC2, 0x17, 0x01, 0xAD },
			{ 0x28, 0xFF, 0x03, 0xB7, 0xC2, 0x17, 0x01, 0xD1 },
			{ 0x28, 0xFF, 0x76, 0xC7, 0xC0, 0x17, 0x01, 0x89 },
			{ 0x28, 0xFF, 0xAB, 0xB0, 0xC2, 0x17, 0x01, 0x5A },
			{ 0x28, 0xFF, 0xC4, 0xAD, 0xC2, 0x17, 0x01, 0x81 },
			{ 0x28, 0xFF, 0xA6, 0x40, 0xC0, 0x17, 0x02, 0x6A },
			{ 0x28, 0xFF, 0x34, 0x99, 0xC0, 0x17, 0x01, 0x2E },
			{ 0x28, 0xFF, 0x38, 0x17, 0x60, 0x17, 0x05, 0x54 } };
// @formatter:on

	uint32_t expect[] { 1294211458, 3932845497, 3159888747, 3312979629,
			1397168482, 3822322055, 4232230555, 3107815502 };

	for (uint8_t i = 0; i < 8; i++) {
		result = calcCrc.crcCalculate(
				reinterpret_cast<uint32_t*>(addressTbl[i]), 2);
		BOOST_TEST(result, expect[i]);
	}
}

