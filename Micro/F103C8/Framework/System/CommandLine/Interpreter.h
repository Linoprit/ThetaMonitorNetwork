/*
 * ComLineInterpreter.h
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#ifndef SYSTEM_COMMANDLINE_INTERPRETER_H_
#define SYSTEM_COMMANDLINE_INTERPRETER_H_

#include "ComLineConfig.h"


namespace cLine {

class Interpreter {
public:
	Interpreter();
	virtual ~Interpreter() {};

	bool doit(CmdBufferType comLine);
	// returns uint32 length, i.e. in=6 => out=2
	static uint32_t calcUint32Len(uint32_t sizeInBytes);

private:


};

} /* namespace cLine */

#endif /* SYSTEM_COMMANDLINE_INTERPRETER_H_ */
