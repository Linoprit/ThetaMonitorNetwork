/*
 * commandLine.cpp
 *
 *  Created on: 05.11.2020
 *      Author: harald
 *  Inspired by Cat (https://github.com/ShareCat/STM32CommandLine)
 */

#include <System/CommandLine/CommandLine.h>
#include <Libraries/HelpersLib.h>
#include <new>
#include <Types/AppTypes.h>
#include <System/OsHelpers.h>

#include <System/printf/printf.h>
#include <System/serialPrintf.h>
#include <iostream>
using namespace std;

//#include "cmsis_os.h"

namespace cLine {

void CommandLine::init(void) {
	new (&instance()) CommandLine();
}

CommandLine& CommandLine::instance(void) {
	static CommandLine commandLine;
	return commandLine;
}

CommandLine::CommandLine() :
	_keyBuffer(KEY_BUFFER_LEN), _cmdPos(0) {
	_cmdBuffer.fill('\0');
	termDisplayClear();
}

void CommandLine::splash(void) {
	termHighLight();
	termResetCursor();
	printf_("\n\n*****************************************\n");
	printf_("**    Stm32 Command Line Interpreter   **\n");
	printf_("**            V1.0 / 2021              **\n");
	printf_("*****************************************\n");
	termUnHighLight();
	printf_("\n");
	termPrompt();
}

void CommandLine::cycle(void) {
	uint8_t actChar = '\0';

	while (readNextChar(actChar) == SUCCESS) {
		if (actChar == _KEY_ENTER) {
			procEnter();
		} else if (actChar == _KEY_BACKSPACE) {
			procBackspace();
		} else if (actChar == _KEY_ESCAPE) {
			int result = readNextChar(actChar);
			if ((result == SUCCESS) && (actChar == _KEY_SQBRACELEFT)) {
				procSqrEscKeys();
			}
			// Minicom sends different END-key sequence
			else if ((result == SUCCESS) && (actChar == _KEY_END_MINICOM1)) {
				result = readNextChar(actChar);
				if ((result == SUCCESS) && (actChar == _KEY_END_MINICOM2)) {
					procEnd();
				}
			} else {
				procEscape();
			}
		} else {
			accumulateChar(actChar);
		}
	}
}

int CommandLine::readNextChar(uint8_t &chr) {
	if (!_keyBuffer.isEmpty()) {
		chr = _keyBuffer.dequeue();
		return SUCCESS;
	}
	return FAIL;
}

void CommandLine::accumulateChar(uint8_t chr) {
	if (_cmdPos == COMMAND_BUFFER_LEN - 1) {
		return;
	}
	if (_cmdBuffer.at(_cmdPos + 1) != '\0') {
		moveCmdRight(_cmdPos);
	}
	_cmdBuffer.at(_cmdPos) = chr;
	incCmdPos();
	termInsert(1u);
	tx_printBuff(&chr, 1);
}

void CommandLine::procSqrEscKeys(void) {
	uint8_t actChar = '\0';
	int result = readNextChar(actChar);

	if ((result == SUCCESS) && (actChar == _KEY_UP)) {
		procArrowUp();
	} else if ((result == SUCCESS) && (actChar == _KEY_DOWN)) {
		procArrowDown();
	} else if ((result == SUCCESS) && (actChar == _KEY_LEFT)) {
		procArrowLeft();
	} else if ((result == SUCCESS) && (actChar == _KEY_RIGHT)) {
		procArrowRight();
	}

	procFourByteEscKeys(actChar);
}

void CommandLine::procFourByteEscKeys(uint8_t actChar) {
	uint8_t seqSuffix = '\0';
	uint8_t result = readNextChar(seqSuffix);

	if ((result == SUCCESS) && (seqSuffix == _SEQ_SUFFIX)) {
		if ((result == SUCCESS) && (actChar == _KEY_POS1)) {
			procPos1();
		} else if ((result == SUCCESS) && (actChar == _KEY_DEL)) {
			procDel();
		} else if ((result == SUCCESS) && (actChar == _KEY_END)) {
			procEnd();
		} else if ((result == SUCCESS) && (actChar == _KEY_PGUP)) {
			procPGUP();
		} else if ((result == SUCCESS) && (actChar == _KEY_PGDN)) {
			procPGDN();
		}
	}
}

void CommandLine::moveCmdRight(uint8_t startPos) {
	uint8_t actPos = startPos;
	uint8_t actChar = _cmdBuffer.at(actPos);

	while (_cmdBuffer.at(actPos) != '\0') {
		uint8_t nextChar = _cmdBuffer.at(actPos + 1);
		_cmdBuffer.at(actPos + 1) = actChar;
		actChar = nextChar;
		actPos++;
	}
}

void CommandLine::moveCmdLeft(uint8_t startPos) {
	uint8_t actPos = startPos;
	while (_cmdBuffer.at(actPos) != '\0') {
		_cmdBuffer.at(actPos) = _cmdBuffer.at(actPos + 1);
		actPos++;
	}
}

void CommandLine::procEnter(void) {
	_history.add(_cmdBuffer);
	_history.resShowPos();

	bool result = _interpret.doit(_cmdBuffer);
	if (!result) {
		printf_("\nUnknown command.\n\0");

	} else {
		printf_("\n** DONE **\n\0"); //  remember TI-99/4A?
	}
	termPrompt();
	resCmdPos();
	_cmdBuffer.fill('\0');
}

void CommandLine::procBackspace(void) {
	if (_cmdPos != 0) {
		decCmdPos();
		termMoveLeft(1u);
		termDelete(1u);
		moveCmdLeft(_cmdPos);
	}
}

void CommandLine::procDel(void) {
	moveCmdLeft(_cmdPos);
	termDelete(1u);
}

void CommandLine::syncCmdPos(void) {
	_cmdPos = 0;
	while ((_cmdPos < COMMAND_BUFFER_LEN) && (_cmdBuffer.at(_cmdPos) != '\0')) {
		incCmdPos();
	}
}

void CommandLine::procArrowUp(void) {
	if (_history.getInsertPos() == _history.getShowPos()) {
		_history.add(_cmdBuffer);
	}
	_cmdBuffer = _history.showUp();
	termPos1();
	termEraseLine(2u);
	termPrompt();
	printf_("%s", _cmdBuffer.data());
	syncCmdPos();
}

void CommandLine::procArrowDown(void) {
	if (_history.getInsertPos() == _history.getShowPos()) {
		return;
	}
	_cmdBuffer = _history.showDown();
	termPos1();
	termEraseLine(2u);
	termPrompt();
	printf_("%s", _cmdBuffer.data());
	syncCmdPos();
}

void CommandLine::procArrowLeft(void) {
	if (_cmdPos != 0) {
		termMoveLeft(1u);
		decCmdPos();
	}
}

void CommandLine::procArrowRight(void) {
	if ((_cmdPos < COMMAND_BUFFER_LEN) && (_cmdBuffer.at(_cmdPos) != '\0')) {
		termMoveRight(1u);
		incCmdPos();
	}
}

void CommandLine::procEnd(void) {
	while ((_cmdPos < COMMAND_BUFFER_LEN) && (_cmdBuffer.at(_cmdPos) != '\0')) {
		termMoveRight(1u);
		incCmdPos();
	}
}

void CommandLine::procPos1(void) {
	termPos1();
	termPrompt();
	resCmdPos();
}

void CommandLine::procEscape(void) {
	// unused
}

void CommandLine::procPGUP(void) {
	// unused
}

void CommandLine::procPGDN(void) {
	// unused
}

} /* namespace comLine */
