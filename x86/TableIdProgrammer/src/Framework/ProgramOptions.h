/*
 * ProgramOptions.h
 *
 *  Created on: 26.05.2021
 *      Author: harald
 */

#ifndef PROGRAMOPTIONS_H_
#define PROGRAMOPTIONS_H_

#include "ProjectTypes.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

class ProgramOptions {
public:
	ProgramOptions() {
	}
	;
	virtual ~ProgramOptions() {
	}
	;
	int init(int argc, char *argv[]);
	po::variables_map getVarMap(void) {return _vm;};

private:
	po::variables_map _vm;

	void checkIfTest(int argc, char *argv[]);
};

#endif /* PROGRAMOPTIONS_H_ */
