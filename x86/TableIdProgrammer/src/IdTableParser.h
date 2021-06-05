/*
 * IdTableParser.h
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#ifndef IDTABLEPARSER_H_
#define IDTABLEPARSER_H_

#include <Framework/ProgramOptions.h>
#include <string>
#include <vector>

using namespace std;

class IdTableParser {
public:

	IdTableParser(po::variables_map varMap, vector<string> fileContent);
	virtual ~IdTableParser() {
	}
	;

	uint32_t calcHash(vector<string> splitted);
	vector<string> splitLine(string line);
	vector<string> getContent(void) { return _content; }
	void dumpContent(void);

private:
	static constexpr int ID_LEN = 8U;
	static constexpr int POS_MIN = 8U;
	static constexpr int POS_MAX = 9U;
	static constexpr int POS_TYPE = 10U;
	static constexpr int POS_RELAY = 11U;
	static constexpr int POS_SHORTNAME = 12U;

	//vector<ID_Table::Theta_sens_type> _content;
	vector<string> _content;
};

#endif /* IDTABLEPARSER_H_ */
