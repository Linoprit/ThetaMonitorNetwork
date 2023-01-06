/*
 * IdTableParser.cpp
 *
 *  Created on: 24.05.2021
 *      Author: harald
 */

#include "IdTableParser.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "CalcCrc.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/format.hpp>

/**
 * Converts the lines of the read file into the command-string.
 */

IdTableParser::IdTableParser(po::variables_map varMap, vector<string> fileContent) {
	for (string line : fileContent) {
		stringstream actItem;

		vector<string> splitted = splitLine(line);
		actItem << varMap["setSensorIdCmd"].as<string>() << " ";
		actItem << to_string(calcHash(splitted)) << " ";
		actItem << splitted.at(POS_MIN) << " ";
		actItem << splitted.at(POS_MAX) << " ";
		actItem << splitted.at(POS_TYPE) << " ";
		actItem << splitted.at(POS_RELAY) << " ";
		boost::trim(splitted.at(POS_SHORTNAME));
		actItem << splitted.at(POS_SHORTNAME);
		actItem << "\r\n";

		// if we want to convert to Theta_sens_type
		//ID_Table::Theta_sens_type actItem;
		//actItem.sensorIdHash = calcHash(splitted);
		//actItem.minVal = std::stof(splitted.at(POS_MIN));
		//actItem.maxVal = std::stof(splitted.at(POS_MAX));
		//actItem.relayNr = std::stoi(splitted.at(POS_RELAY));
		//actItem.sensType = static_cast<ID_Table::SensorType>(std::stoi(
		//		splitted.at(POS_TYPE)));
		//boost::trim(splitted.at(POS_SHORTNAME));
		//memcpy(actItem.shortname, splitted.at(POS_SHORTNAME).c_str(),
		//		ID_Table::SHORTNAME_LEN + 2);

		_content.push_back(actItem.str());
	}
	// dumpContent();
	cout << "Parsing file ok\n";
}

uint32_t IdTableParser::calcHash(vector<string> splitted) {
	CalcCrc calcCrc;
	uint32_t crcInput[2] { 0, 0 };
	uint8_t *crcPtr = reinterpret_cast<uint8_t*>(crcInput);
	for (uint_fast8_t i = 0; i < ID_LEN; i++) {
		boost::trim(splitted.at(i));
		uint8_t number = std::stoi(splitted.at(i), 0, 16);
		crcPtr[i] = number;
	}
	uint32_t result = calcCrc.crcCalculate(crcInput, 2);
	return result;
}

vector<string> IdTableParser::splitLine(string line) {
	vector<string> result;
	boost::split(result, line, boost::is_any_of(","));
	return result;
}

void IdTableParser::dumpContent(void) {
	for (uint i = 0; i < _content.size(); i++) {

		cout << _content.at(i) << std::flush;

		// cout << to_string(_content.at(i).sensorIdHash) << " ";
		// cout << boost::format("%2.1f") % _content.at(i).minVal << " ";
		// cout << boost::format("%2.1f") % _content.at(i).maxVal << " ";
		// cout << to_string(_content.at(i).sensType) << " ";
		// cout << to_string(_content.at(i).relayNr) << " ";
		// cout << string(_content.at(i).shortname, ID_Table::SHORTNAME_LEN + 2)
		// 		<< "\n";
	}
	cout << "\n";
}

