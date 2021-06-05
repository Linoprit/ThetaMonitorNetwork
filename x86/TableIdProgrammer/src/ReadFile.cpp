/*
 * ReadFile.cpp
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#include "ReadFile.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

ReadFile::ReadFile(string filename) {
	string line;
	ifstream myfile(filename);
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			boost::algorithm::trim(line);
			if(isCommentOrEmpty(line))
				continue;
			line = splitComment(line);
			//cout << line << '\n';
			fileContent.push_back(line);
		}
		myfile.close();
		cout << "Reading file ok: " << filename << "\n";
	}
	else
		cout << "Unable to open file: " << filename;
}

string ReadFile::splitComment(string line){
    vector<string> result;
    boost::split(result, line, boost::is_any_of("#"));
    return result.at(0);
}

bool ReadFile::isCommentOrEmpty(string line){
	if (line.empty())
		return true;

	if (line.at(0) == '#')
		return true;

	return false;
}
