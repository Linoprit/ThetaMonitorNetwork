/*
 * Unit_Convert.cpp
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <Libraries/Convert.h>

using ::testing::Expectation;

TEST(Convert, saturate){
	Convert conv_01(0.1F, -12.7F, -12.7F, 12.7F);
	EXPECT_FLOAT_EQ(conv_01.saturatePhys(25.0F), 12.7F);
	EXPECT_FLOAT_EQ(conv_01.saturatePhys(-25.0F), -12.7F);
	EXPECT_FLOAT_EQ(conv_01.saturatePhys(12.8F), 12.7F);
}

TEST(Convert, phys2Int){
	Convert conv_01(0.1F, -12.7F, -12.7F, 12.7F);
	EXPECT_FLOAT_EQ(conv_01.int2Phys(0U), -12.7F);
	EXPECT_FLOAT_EQ(conv_01.int2Phys(255U), 12.7F);
	EXPECT_FLOAT_EQ(conv_01.int2Phys(127U), 0.0F);
}
TEST(Convert, int2Phys){
	Convert conv_01(0.1F, -12.7F, -12.7F, 12.7F);
	EXPECT_EQ(conv_01.phys2Int(-12.7F), 0U);
	EXPECT_EQ(conv_01.phys2Int(12.7F), 254U);
	EXPECT_EQ(conv_01.phys2Int(0.0F), 127U);
	EXPECT_EQ(conv_01.phys2Int(-15.0F), 0U);
	EXPECT_EQ(conv_01.phys2Int(15.0F), 254U);

	EXPECT_EQ(conv_01.phys2Int(2.70F), 154U);
	EXPECT_EQ(conv_01.phys2Int(2.75F), 155U);
	EXPECT_EQ(conv_01.phys2Int(2.850001F), 156U);
}
