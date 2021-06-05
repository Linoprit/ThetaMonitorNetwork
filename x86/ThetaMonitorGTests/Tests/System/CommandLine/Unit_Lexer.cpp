/*
 * Unit_Lexer.cpp
 *
 *  Created on: 12.04.2021
 *      Author: harald
 */
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <System/CommandLine/Lexer.h>
#include <System/CommandLine/ComLineConfig.h>
#include <System/CommandLine/Token.h>
#include <Application/ThetaSensors/ID_Table.h>

using ::testing::Expectation;
using namespace cLine;

TEST(Lexer, command) {
	CmdBufferType comLine;
	char command[] = "  e2dumpjournalmap abc";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	CmdToken *cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getVal(), 2362017329U);
	EXPECT_EQ(cmdToken->getType(), Token::Command);
}

TEST(Lexer, integer) {
	CmdBufferType comLine;
	char command[] = "  12345 abc";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	IntToken *cmdToken = (IntToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getVal(), 12345U);
	EXPECT_EQ(cmdToken->getType(), Token::Integer);
}

TEST(Lexer, signInt) {
	CmdBufferType comLine;
	char command[] = "  -12345 abc";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	SintToken *cmdToken = (SintToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getVal(), -12345);
	EXPECT_EQ(cmdToken->getType(), Token::SignInt);
}

TEST(Lexer, float) {
	CmdBufferType comLine;
	char command[] = "  12345.5678 abc";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	FltToken *cmdToken = (FltToken*) lex.getNextToken();
	EXPECT_FLOAT_EQ(cmdToken->getVal(), 12345.5678F);
	EXPECT_EQ(cmdToken->getType(), Token::Float);
}

TEST(Lexer, floatNeg) {
	CmdBufferType comLine;
	char command[] = "  -12345.5678 abc";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	FltToken *cmdToken = (FltToken*) lex.getNextToken();
	EXPECT_FLOAT_EQ(cmdToken->getVal(), -12345.5678F);
	EXPECT_EQ(cmdToken->getType(), Token::Float);
}

TEST(Lexer, string) {
	CmdBufferType comLine;
	char command[] = "  \"einszwo\"  ";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	ChrToken *cmdToken = (ChrToken*) lex.getNextToken();
	std::string result(cmdToken->getVal());
	std::string ref("einszwo");
	EXPECT_TRUE(ref == result);
	EXPECT_EQ(cmdToken->getType(), Token::String);
}

TEST(Lexer, stringAllChars) {
	CmdBufferType comLine;
	char command[] = "  \"#&[]()@+{};:<>|~\"  ";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	ChrToken *cmdToken = (ChrToken*) lex.getNextToken();
	std::string result(cmdToken->getVal());
	std::string ref("#&[]()@+{};:<>|~");
	EXPECT_TRUE(ref == result);
	EXPECT_EQ(cmdToken->getType(), Token::String);
}

TEST(Lexer, stringOverflow) {
	CmdBufferType comLine;
	char command[] = "  \"einszwodreivierfuenfsechssiebenacht\"  ";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	ChrToken *cmdToken = (ChrToken*) lex.getNextToken();
	std::string result(cmdToken->getVal());
	std::string ref("einszwodreivierfuenfsechssieben");
	EXPECT_TRUE(ref == result);
	EXPECT_EQ(cmdToken->getType(), Token::String);
}

TEST(Lexer, stringNotClosed) {
	CmdBufferType comLine;
	char command[] = "  \"einszwodrei  vier";
	memcpy(comLine.data(), command, sizeof(command));

	Lexer lex(&comLine);
	ChrToken *cmdToken = (ChrToken*) lex.getNextToken();
	EXPECT_TRUE(*cmdToken->getVal() == '\0');
	EXPECT_EQ(cmdToken->getType(), Token::String);
}

ID_Table::Theta_sens_type getThetaStruct(Lexer *lex) {
	ID_Table::Theta_sens_type sens;

	IntToken *intToken = (IntToken*) lex->getNextToken();
	EXPECT_EQ(intToken->getType(), Token::Integer);
	sens.sensorIdHash = intToken->getVal();

	FltToken *fltToken = (FltToken*) lex->getNextToken();
	EXPECT_FLOAT_EQ(fltToken->getType(), Token::Float);
	sens.minVal = fltToken->getVal();

	fltToken = (FltToken*) lex->getNextToken();
	EXPECT_FLOAT_EQ(fltToken->getType(), Token::Float);
	sens.maxVal = fltToken->getVal();

	intToken = (IntToken*) lex->getNextToken();
	EXPECT_EQ(intToken->getType(), Token::Integer);
	sens.sensType = static_cast<ID_Table::SensorType>(intToken->getVal());

	intToken = (IntToken*) lex->getNextToken();
	EXPECT_EQ(intToken->getType(), Token::Integer);
	sens.relayNr = intToken->getVal();

	ChrToken *chrToken = (ChrToken*) lex->getNextToken();
	EXPECT_EQ(chrToken->getType(), Token::String);
	memset(sens.shortname, ' ', ID_Table::SHORTNAME_LEN);
	memcpy(sens.shortname, chrToken->getVal(), ID_Table::SHORTNAME_LEN);

	return sens;
}

TEST(Lexer, commandLine) {
	CmdBufferType comLine;
	char command1[] = " setSensId 3103548024 -10.0	10.0	0	0 \"INNEN   \" ";
	memcpy(comLine.data(), command1, sizeof(command1));
	Lexer lex(&comLine);

	CmdToken *cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	uint32_t cmd = cmdToken->getVal();
	EXPECT_EQ(2358927868U, cmd);

	ID_Table::Theta_sens_type sens = getThetaStruct(&lex);
	EXPECT_EQ(3103548024U, sens.sensorIdHash);
	EXPECT_FLOAT_EQ(-10.0F, sens.minVal);
	EXPECT_FLOAT_EQ(10.0F, sens.maxVal);
	EXPECT_EQ(0, sens.sensType);
	EXPECT_EQ(0, sens.relayNr);
	std::string value1(sens.shortname, ID_Table::SHORTNAME_LEN);
	std::string ref1("INNEN   ", ID_Table::SHORTNAME_LEN);
	EXPECT_TRUE(ref1 == value1);

	char command2[] = " setSensId 1294211458 -2.0 10.0  0 1 \"WST_O   \" ";
	comLine.fill('\0');
	memcpy(comLine.data(), command2, sizeof(command2));
	lex.setComLine(&comLine);

	cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	cmd = cmdToken->getVal();
	EXPECT_EQ(2358927868U, cmd);

	sens = getThetaStruct(&lex);
	EXPECT_EQ(1294211458U, sens.sensorIdHash);
	EXPECT_FLOAT_EQ(-2.0F, sens.minVal);
	EXPECT_FLOAT_EQ(10.0F, sens.maxVal);
	EXPECT_EQ(0, sens.sensType);
	EXPECT_EQ(1, sens.relayNr);
	std::string value2(sens.shortname, ID_Table::SHORTNAME_LEN);
	std::string ref2("WST_O   ", ID_Table::SHORTNAME_LEN);
	EXPECT_TRUE(ref2 == value2);

	char command3[] = "setSensId 3932845497 -4.2 9.8 0 1 \"WST_U   \" ";
	comLine.fill('\0');
	memcpy(comLine.data(), command3, sizeof(command3));
	lex.setComLine(&comLine);

	cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	cmd = cmdToken->getVal();
	EXPECT_EQ(2358927868U, cmd);

	sens = getThetaStruct(&lex);
	EXPECT_EQ(3932845497U, sens.sensorIdHash);
	EXPECT_FLOAT_EQ(-4.2F, sens.minVal);
	EXPECT_FLOAT_EQ(9.8F, sens.maxVal);
	EXPECT_EQ(0, sens.sensType);
	EXPECT_EQ(1, sens.relayNr);
	std::string value3(sens.shortname, ID_Table::SHORTNAME_LEN);
	std::string ref3("WST_U   ", ID_Table::SHORTNAME_LEN);
	EXPECT_TRUE(ref3 == value3);

	char command4[] = "setSensId	3159888747 -8.56 7.75 10 1	\"GGE_O   \" ";
	comLine.fill('\0');
	memcpy(comLine.data(), command4, sizeof(command4));
	lex.setComLine(&comLine);

	cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	cmd = cmdToken->getVal();
	EXPECT_EQ(2358927868U, cmd);

	sens = getThetaStruct(&lex);
	EXPECT_EQ(3159888747U, sens.sensorIdHash);
	EXPECT_FLOAT_EQ(-8.56F, sens.minVal);
	EXPECT_FLOAT_EQ(7.75F, sens.maxVal);
	EXPECT_EQ(10, sens.sensType);
	EXPECT_EQ(1, sens.relayNr);
	std::string value4(sens.shortname, ID_Table::SHORTNAME_LEN);
	std::string ref4("GGE_O   ", ID_Table::SHORTNAME_LEN);
	EXPECT_TRUE(ref4 == value4);
}

TEST(Lexer, printCmdHashes) {
	char getSensIdTable[] = "getSensIdTable";
	char setStationId[] = "setStationId";
	char getStationId[] = "getStationId";
	char reboot[] = "reboot";

	CmdBufferType comLine;
	comLine.fill('\0');
	memcpy(comLine.data(), getSensIdTable, sizeof(getSensIdTable));
	Lexer lex(&comLine);

	CmdToken *cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	printf("getSensIdTable: %u\n", cmdToken->getVal());

	comLine.fill('\0');
	memcpy(comLine.data(), setStationId, sizeof(setStationId));
	lex.setComLine(&comLine);
	cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	printf("setStationId: %u\n", cmdToken->getVal());

	comLine.fill('\0');
	memcpy(comLine.data(), getStationId, sizeof(getStationId));
	lex.setComLine(&comLine);
	cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	printf("getStationId: %u\n", cmdToken->getVal());

	comLine.fill('\0');
	memcpy(comLine.data(), reboot, sizeof(reboot));
	lex.setComLine(&comLine);
	cmdToken = (CmdToken*) lex.getNextToken();
	EXPECT_EQ(cmdToken->getType(), Token::Command);
	printf("reboot: %u\n", cmdToken->getVal());
}
