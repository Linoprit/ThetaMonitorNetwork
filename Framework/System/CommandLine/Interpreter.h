/*
 * ComLineInterpreter.h
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#ifndef SYSTEM_COMMANDLINE_INTERPRETER_H_
#define SYSTEM_COMMANDLINE_INTERPRETER_H_

#include <System/CommandLine/Lexer.h>
#include "ComLineConfig.h"

namespace cLine {

class Interpreter {
public:
	Interpreter();
	virtual ~Interpreter() {
	}
	;

	bool doit(CmdBufferType comLine);

private:
	bool setSensId(Lexer *lex);
	bool getSensIdTable(Lexer *lex);
	bool setStationId(Lexer *lex);
	bool getStationId(Lexer *lex);
	bool clrSensIdTable(Lexer *lex);
	bool calcHash(Lexer *lex);
};

} /* namespace cLine */

#endif /* SYSTEM_COMMANDLINE_INTERPRETER_H_ */
