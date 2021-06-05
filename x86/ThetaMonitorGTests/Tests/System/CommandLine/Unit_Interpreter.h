/*
 * Unit_Interpreter.h
 *
 *  Created on: 21.03.2021
 *      Author: harald
 */

#ifndef TESTS_SYSTEM_COMMANDLINE_UNIT_INTERPRETER_H_
#define TESTS_SYSTEM_COMMANDLINE_UNIT_INTERPRETER_H_
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stm32f1xx.h>
#include <System/CommandLine/Interpreter.h>
#include <System/CommandLine/ComLineConfig.h>
#include <System/CommandLine/Lexer.h>

using namespace cLine;
//@formatter:off
class MockDispatcher: public Lexer {
public:

	MOCK_METHOD0( PenUp, void());
	MOCK_METHOD0(PenDown, void());
	MOCK_METHOD1(Forward, void(int distance));
	MOCK_METHOD1(Turn, void(int degrees));
	MOCK_METHOD2(GoTo, void(int x, int y));
	MOCK_CONST_METHOD0(GetX, int());
	MOCK_CONST_METHOD0(GetY, int());

};
//@formatter:on




#endif /* TESTS_SYSTEM_COMMANDLINE_UNIT_INTERPRETER_H_ */
