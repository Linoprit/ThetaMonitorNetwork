/*
 * ReadFile.h
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#ifndef READFILE_H_
#define READFILE_H_

#include <string>
#include <vector>

using namespace std;

class ReadFile {
public:
	ReadFile(string filename);
	virtual ~ReadFile() {};

	vector<string> getContent(void){ return fileContent; };
	bool isCommentOrEmpty(string line);
	string splitComment(string line);

private:
	vector<string> fileContent;

};

#endif /* READFILE_H_ */
