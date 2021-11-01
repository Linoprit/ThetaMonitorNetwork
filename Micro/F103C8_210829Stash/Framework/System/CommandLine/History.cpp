/*
 * ComLineHistory.cpp
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#include <System/CommandLine/History.h>

namespace cLine {

History::History():
	_insertPos(0),
	_showPos(0)
{
	CmdBufferType emptyLine;
	emptyLine.fill('\0');
	for (uint8_t i=0; i < HISTORY_DEPTH; i++) {
		_histArray.fill(emptyLine);
	}
}

bool History::add(CmdBufferType comLine) {
	uint8_t count = _insertPos;
	decPos(count);
	if((comlineIsEmpty(comLine)) ||
			(comLineIsEqual(comLine, _histArray.at(count))) ) {
		return false;
	}

	_histArray.at(_insertPos) = comLine;
	incInsertPos();
	return true;
}

bool History::comLineIsEqual(CmdBufferType left, CmdBufferType right) {
	for(uint8_t i=0; i < left.size(); i++){
		if(left.at(i) != right.at(i)){
			return false;
		}
	}
	return true;
}

CmdBufferType History::showUp(void) {
	uint8_t lastVal = _showPos;
	decShowPos();
	if( (_showPos == _insertPos) ||
			(comlineIsEmpty(_histArray.at(_showPos))) ) {
		_showPos = lastVal;
	}
	return _histArray.at(_showPos);
}

CmdBufferType History::showDown(void) {
	uint8_t lastVal = _showPos;
	incShowPos();
	if(_showPos == _insertPos) {
		_showPos = lastVal;
	}
	return _histArray.at(_showPos);
}

bool History::comlineIsEmpty(CmdBufferType comLine) {
	return (comLine.at(0) == '\0');
}

void History::incPos(uint8_t& pos) {
	if(pos < HISTORY_DEPTH-1) {
		pos++;
	} else {
		pos = 0;
	}
}

void History::decPos(uint8_t& pos) {
	if(pos > 0) {
		pos--;
	} else {
		pos = HISTORY_DEPTH -1u;
	}
}

void History::resPos(uint8_t& pos) {
	pos = 0;
}


} /* namespace cLine */
