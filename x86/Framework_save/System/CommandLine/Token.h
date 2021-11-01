/*
 * Token.h
 *
 *  Created on: 12.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_SYSTEM_COMMANDLINE_TOKEN_H_
#define FRAMEWORK_SYSTEM_COMMANDLINE_TOKEN_H_

#include <stdint.h>
#include <string>

namespace cLine {

class Token {
public:
	enum TokenType {Command, String, Integer, SignInt, Float, None };

	Token(): _type { None } {};
	Token(TokenType type): _type { type } {};
	virtual ~Token() {};

	TokenType getType(void) { return _type; };

protected:
	TokenType _type;
};

template <class _Tp, Token::TokenType type>
class TToken: public Token {
public:
	TToken(): Token(None), _value { 0 } {};
	TToken(_Tp value): Token(type), _value { value } {};
	virtual ~TToken() {};

	_Tp getVal(void) { return _value; };

private:
	_Tp _value;
};

typedef TToken<uint32_t, Token::TokenType::Command> CmdToken;
typedef TToken<uint32_t, Token::TokenType::Integer> IntToken;
typedef TToken<int32_t, Token::TokenType::SignInt> SintToken;
typedef TToken<float, Token::TokenType::Float> FltToken;
typedef TToken<char*, Token::TokenType::String> ChrToken;

} /* namespace cLine */

#endif /* FRAMEWORK_SYSTEM_COMMANDLINE_TOKEN_H_ */
