/*
 * ComLineInterpreter.cpp
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#include <System/CommandLine/Interpreter.h>
#include <Sockets/CrcSocket.h>
#include <System/serialPrintf.h>
#include <cstring>
#include <Application/ThetaSensors/ThetaMeasurement.h>
#include <Application/ThetaSensors/ID_Table.h>
#include <System/OsHelpers.h>

namespace cLine {

Interpreter::Interpreter() {
}

bool Interpreter::doit(CmdBufferType comLine) {
	bool result = false;
	Lexer lex(&comLine);

	CmdToken *cmdToken = (CmdToken*) lex.getNextToken();
	if (cmdToken->getType() != Token::Command) {
		return false;
	}
	uint32_t cmd = cmdToken->getVal();

//@formatter:off
	if (cmd == 2358927868U)     { result = setSensId(&lex); 		} // setSensId
	else if (cmd == 3913104177) { result = getSensIdTable(&lex); 	} // getSensIdTable
	else if (cmd == 1925253141) { result = setStationId(&lex); 		} // setStationId
	else if (cmd == 2948963465) { result = getStationId(&lex); 		} // getStationId
	else if (cmd == 1676458703) { OsHelpers::SYSTEM_REBOOT();		} // reboot
	else if (cmd == 1973435441) { OsHelpers::SYSTEM_EXIT();			} // exit
//@formatter:on

	return result;
}

bool Interpreter::setStationId(Lexer *lex) {

	IntToken *intToken = (IntToken*) lex->getNextToken();
	if (intToken->getType() != Token::Integer) {
		return false;
	}
	msmnt::ThetaMeasurement::instance().getNonVolatileData().writeStatId(
			intToken->getVal());
	return true;
}

bool Interpreter::getSensIdTable(Lexer *lex) {
	printf("Printing sensor-table from E2:\n");
	msmnt::ThetaMeasurement::instance().getNonVolatileData().printIdTableRaw();
	return true;
}

bool Interpreter::getStationId(Lexer *lex) {
	uint32_t stationId =
			msmnt::ThetaMeasurement::instance().getNonVolatileData().getStatId();
	tx_printf("\nStation ID = %u\n", stationId);
	return true;
}

bool Interpreter::setSensId(Lexer *lex) {
	ID_Table::Theta_sens_type sens;

	IntToken *intToken = (IntToken*) lex->getNextToken();
	if (intToken->getType() != Token::Integer) {
		return false;
	}
	sens.sensorIdHash = intToken->getVal();

	FltToken *fltToken = (FltToken*) lex->getNextToken();
	if (fltToken->getType() != Token::Float) {
		return false;
	}
	sens.minVal = fltToken->getVal();

	fltToken = (FltToken*) lex->getNextToken();
	if (fltToken->getType() != Token::Float) {
		return false;
	}
	sens.maxVal = fltToken->getVal();

	intToken = (IntToken*) lex->getNextToken();
	if (intToken->getType() != Token::Integer) {
		return false;
	}
	sens.sensType = static_cast<ID_Table::SensorType>(intToken->getVal());

	intToken = (IntToken*) lex->getNextToken();
	if (intToken->getType() != Token::Integer) {
		return false;
	}
	sens.relayNr = intToken->getVal();

	ChrToken *chrToken = (ChrToken*) lex->getNextToken();
	if (chrToken->getType() != Token::String) {
		return false;
	}
	memset(sens.shortname, ' ', ID_Table::SHORTNAME_LEN);
	memcpy(sens.shortname, chrToken->getVal(), ID_Table::SHORTNAME_LEN);

	//ErrorCode result =
			msmnt::ThetaMeasurement::instance().getNonVolatileData().writeIdTableData(
					sens);

	return true;
}

} /* namespace cLine */
