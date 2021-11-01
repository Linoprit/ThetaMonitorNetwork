/*
 * dispatcher.cpp
 *
 *  Created on: 21.03.2021
 *      Author: harald
 */

#include <System/CommandLine/Lexer.h>
#include <System/serialPrintf.h>
#include <string>
#include <Sockets/CrcSocket.h>

namespace cLine {

char strValueBuff[Lexer::STRING_BUFF_LEN];
char tokenBuff[sizeof(ChrToken)]; // chrToken is the biggest one

Lexer::Lexer(CmdBufferType *comLine) {
	setComLine(comLine);
}

void Lexer::setComLine(CmdBufferType *comLine) {
	_comLine = comLine;
	_actPos = 0;
	_currentChar = _comLine->at(0);
}

Token* Lexer::getNextToken(void) {
	while (_currentChar != '\0') {
		if (isspace(_currentChar)) {
			skipWhitespace();
			continue;
		}
		if (_currentChar == '\"') {
			std::string str = makeStr();
			memset(strValueBuff, '\0', Lexer::STRING_BUFF_LEN);
			memcpy(strValueBuff, const_cast<char*>(str.c_str()), str.size());
			return new (tokenBuff) ChrToken(strValueBuff);
		}
		if (isdigit(_currentChar) || (_currentChar == '-')) {
			return makeNum();
		}
		if (isalpha(_currentChar)) {
			return new (tokenBuff) CmdToken(makeCmd());
		}
		break; // all unknown chars, like ','
	}
	return new (tokenBuff) Token(Token::TokenType::None);
}

void Lexer::advance(void) {
	_actPos += 1;
	if (_actPos > _comLine->size() - 1) {
		_currentChar = '\0';  // Indicates end of input
	} else {
		_currentChar = _comLine->at(_actPos);
	}
}

void Lexer::skipWhitespace(void) {
	while (isspace(_currentChar)) {
		advance();
	}
}

Token* Lexer::makeNum(void) {
	bool isInteger = true;
	bool isSigned = false;
	uint_fast8_t startPos = _actPos;

	while (isdigit(_currentChar) || (_currentChar == '.')
			|| (_currentChar == '-')) {
		if (_currentChar == '.') {
			isInteger = false;
		}
		if (_currentChar == '-') {
			isSigned = true;
		}
		advance();
	}
	std::string::size_type sz = _actPos - startPos;
	uint8_t *startChar = _comLine->data() + startPos;

	if ((isInteger) && (isSigned == false)) {
		uint32_t result = std::stol(reinterpret_cast<char*>(startChar), &sz);
		return new (tokenBuff) IntToken(result);
	} else if ((isInteger) && (isSigned)) {
		int32_t result = std::stol(reinterpret_cast<char*>(startChar), &sz);
		return new (tokenBuff) SintToken(result);
	}
	float fltResult = std::stof(reinterpret_cast<char*>(startChar), &sz);
	return new (tokenBuff) FltToken(fltResult);
}

std::string Lexer::makeStr(void) {
	advance(); // go over string opening char (")
	uint_fast8_t startPos = _actPos;
	uint_fast8_t endPos = _actPos;
	while ((_currentChar >= ' ') && (_currentChar <= '~')
			&& (_currentChar != '\0')) {
		if (_currentChar == '\"') {
			endPos = _actPos;
			advance(); // go over string closing char (")
			break;
		}
		advance();
	}
	std::string::size_type sz = endPos - startPos;
	if (sz > (STRING_BUFF_LEN - 1)) {
		sz = STRING_BUFF_LEN - 1; // keep space for final '\0'
	}
	uint8_t *startChar = _comLine->data() + startPos;
	std::string result(reinterpret_cast<char*>(startChar), sz);
	return result;
}

uint32_t Lexer::makeCmd(void) {
	uint32_t hashCode = 0;
	uint_fast8_t startPos = _actPos;
	while (isalnum(_currentChar) && (_currentChar != '\0')
			&& (_currentChar != ' ')) {
		advance();
	}
	std::string::size_type sz = _actPos - startPos;
	uint8_t *startChar = _comLine->data() + startPos;

	uint32_t len32 = CrcSocket::calcUint32Len(sz);
	uint8_t commandBuf[len32 * 4];
	memset(commandBuf, '\0', len32 * 4);
	memcpy(commandBuf, startChar, sz);
	hashCode = CrcSocket::calc_chksum32(reinterpret_cast<uint32_t*>(commandBuf),
			len32);

	return hashCode;
}

/*bool Lexer::isAlphaNum(unsigned char Char) {
 return ((isAlpha(Char)) || (isDigit(Char)));
 }

 bool Lexer::isAlpha(unsigned char Char) {
 return (((Char >= 'a') && (Char <= 'z')) || ((Char >= 'A') && (Char <= 'Z')));
 }

 bool Lexer::isDigit(unsigned char Char) {
 return ((Char >= '0') && (Char <= '9'));
 }

 bool Lexer::isNone(unsigned char Char) {
 return (Char == '\0');
 }

 bool Lexer::isSpace(unsigned char Char) {
 return (Char == ' ');
 }*/

}
/* namespace cLine */
