/*
 * dispatcher.h
 *
 *  Created on: 21.03.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_SYSTEM_COMMANDLINE_LEXER_H_
#define FRAMEWORK_SYSTEM_COMMANDLINE_LEXER_H_

#include "Token.h"
#include "ComLineConfig.h"

namespace cLine {

class Lexer {
public:
	static constexpr uint8_t STRING_BUFF_LEN = 32;

	Lexer(CmdBufferType* comLine);
	virtual ~Lexer() {};

	void setComLine(CmdBufferType* comLine);
	Token* getNextToken(void);

private:
	CmdBufferType* _comLine;
	uint_fast8_t _actPos;
	unsigned char _currentChar;

	void advance(void);
	void skipWhitespace(void);

	Token* makeNum(void);
	std::string makeStr(void);
	uint32_t makeCmd(void);
};

} /* namespace cLine */

#endif /* FRAMEWORK_SYSTEM_COMMANDLINE_LEXER_H_ */
