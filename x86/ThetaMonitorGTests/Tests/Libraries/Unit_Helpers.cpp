/*
 * Unit_Helpers.cpp
 *
 *  Created on: 04.11.2021
 *      Author: harald
 */
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <Libraries/HelpersLib.h>

using ::testing::Expectation;
using namespace std;

TEST(Helpers, floatToStr) {
	float someVal = 12345.678; // rounding
	string result = HelpersLib::floatToStr(someVal, 2u);
	EXPECT_EQ(result, string("12345.68"));

	someVal = -12345.678529; // rounding
	result = HelpersLib::floatToStr(someVal, 3u);
	EXPECT_EQ(result, string("-12345.679"));

	someVal = -0.678429;
	result = HelpersLib::floatToStr(someVal, 5u);
	EXPECT_EQ(result, string("-0.67843"));

	// Remember a float32 uses 24 bits in its mantissa part,
	// this results in a precicion-loss, if mantissa > 33554430u
	// (0xFFFFFF)
	someVal = 33554430.0f;
	result = HelpersLib::floatToStr(someVal, 0u);
	EXPECT_EQ(result, string("33554430")); // must work

	// precision loss, if this test fails, comment it out!
	// someVal = 33554433.0f;
	// result = HelpersLib::floatToStr(someVal, 0u);
	// EXPECT_EQ(result, string("33554432")); // last place is undefined!

	// We are above the precission. UINT32_MAX is 0xFFFFFFFF, the conversion will
	// cut the last byte, so every value above 0xFFFFFF (4294967040d) is undefined.
	// Same for the part after decimal point. We will return "####" in that cases.
	someVal = 4294967295.0f;
	result = HelpersLib::floatToStr(someVal, 0u);
	EXPECT_EQ(result, string("####"));

	someVal = 4294967040.0f;
	result = HelpersLib::floatToStr(someVal, 0u);
	EXPECT_EQ(result, string("4294967040"));

	someVal = 4294967040.4294967040; // precision loss
	result = HelpersLib::floatToStr(someVal, 4u);
	EXPECT_EQ(result, string("4294967040.0000"));

	someVal = -777.2125f; // rounding
	result = HelpersLib::floatToStr(someVal, 3u);
	EXPECT_EQ(result, string("-777.213"));

	someVal = 777.2125f; // rounding
	result = HelpersLib::floatToStr(someVal, 3u);
	EXPECT_EQ(result, string("777.213"));
}

// playing around with the bits in the IEEE float32
// someVal = 16777216.0;
// uint32_t *fptr = reinterpret_cast<uint32_t*>(&someVal);
// printf("float: %.5f / cast: %x\n", someVal, *fptr);
// *fptr = 0x4BFFFFFF;
// printf("float: %.5f / cast: %x\n", someVal, *fptr);

