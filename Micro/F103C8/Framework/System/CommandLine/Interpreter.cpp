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

namespace cLine {

Interpreter::Interpreter() {
	//float someVal = std::stof("123.1");

}

uint32_t Interpreter::calcUint32Len(uint32_t sizeInBytes) {
	uint32_t len32 = (sizeInBytes + 3) / 4;
	return len32;
}

bool Interpreter::doit(CmdBufferType comLine) {
	uint32_t hashCode = 0;
	uint8_t i = 0;
	uint8_t commandBuf[COMMAND_BUFFER_LEN];
	memset(commandBuf, '\0', COMMAND_BUFFER_LEN);

	for (i = 0; i < COMMAND_BUFFER_LEN; i++) {
		uint8_t actChar = comLine.at(i);
		if ((actChar == ' ') || (actChar == '\0')) {
			break;
		}
		commandBuf[i] = actChar;
	}

	//if (i > 0) {
	// adjust size to fit into uint32 type
	uint32_t len32 = calcUint32Len(i);
	hashCode = CrcSocket::calc_chksum32(reinterpret_cast<uint32_t*>(commandBuf),
			len32);
	tx_printf("\n%lu / %i / %i:\" %s\"\n", hashCode, i, len32, commandBuf);
	//}

	// here we could look for arguments, starting at i
	// uint8_t oldPos = i;
	// oldPos++; // was positioned at first space
	// std::string argStr;
	// for ( i = oldPos; i < COMMAND_BUFFER_LEN; i++) {
	// 	uint8_t actChar = comLine.at(i);
	// 	if( (actChar == ' ') || (actChar == '\0') ) {
	// 		break;
	// 	}
	// 	argStr.insert(i, actChar, 1);
	// }
	// uint32_t argVal = std::stoi(argStr);

	return true;
}

} /* namespace cLine */
