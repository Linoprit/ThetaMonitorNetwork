/*
 * ComLineHistory.h
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#ifndef SYSTEM_COMLINEHISTORY_H_
#define SYSTEM_COMLINEHISTORY_H_

#include "ComLineConfig.h"

namespace cLine {

class History {
public:
	History();
	virtual ~History() {};

	bool add(CmdBufferType comLine);
	CmdBufferType showUp(void);
	CmdBufferType showDown(void);
	void reset();

	uint8_t getInsertPos() const { return _insertPos;	}
	uint8_t getShowPos() const 	 { return _showPos; 	}
	void incShowPos(void) {	incPos(_showPos); 	}
	void decShowPos(void) { decPos(_showPos); 	}
	void resShowPos(void) { _showPos = _insertPos;	}
	bool comlineIsEmpty(CmdBufferType comLine);
	bool comLineIsEqual(CmdBufferType left, CmdBufferType right);

private:
	std::array<CmdBufferType, HISTORY_DEPTH> _histArray;
	uint8_t _insertPos;
	uint8_t _showPos;

	void incPos(uint8_t& pos);
	void decPos(uint8_t& pos);
	void resPos(uint8_t& pos);

	void incInsertPos(void) { incPos(_insertPos); }
	void decInsertPos(void) { decPos(_insertPos); }

};

} /* namespace cLine */

#endif /* SYSTEM_COMLINEHISTORY_H_ */
