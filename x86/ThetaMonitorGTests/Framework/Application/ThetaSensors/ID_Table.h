/*
 * ID_Table.h
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#ifndef THETASENSORS_ID_TABLE_H_
#define THETASENSORS_ID_TABLE_H_

#include <stdint.h>
#include <string>

#ifdef __x86_64
#include <iostream>
#include <boost/format.hpp>
using namespace std;
#endif


class ID_Table {
public:
	static constexpr uint16_t ID_TABLE_LEN = 22;
	static constexpr uint16_t SHORTNAME_LEN = 8;
	static constexpr uint16_t INVLD_TEMPERATURE = 255;
	static constexpr uint16_t MIN_TEMP_HYSTERESIS = 1;
	static constexpr uint16_t SENSOR_ID_LEN = 8;

	enum SensorType {
		TEMP = 0, HUMIDITY = 1, PRESS = 2
	};
	enum StationType {
		MASTER = 3, SLAVE_01 = 2, SLAVE_02 = 1, SLAVE_03 = 0
	};

	typedef struct {
		uint32_t sensorIdHash;
		float minVal;
		float maxVal;
		SensorType sensType;
		uint8_t relayNr; // 0 = no relay
		char shortname[ID_Table::SHORTNAME_LEN];
	} Theta_sens_type; // to be used in application

	ID_Table();
	virtual ~ID_Table() {
	}
	;

	static std::string sensorType2Str(SensorType sensorType);
	static std::string sensorType2Str(uint8_t sensorType);

	/*static const uint8_t get_stationNo(uint8_t sensor_ID[SENSOR_ID_LEN]);
	 static const uint8_t get_relayNo(uint8_t sensor_ID[SENSOR_ID_LEN]);
	 static const char*   get_shortname(uint8_t sensor_ID[SENSOR_ID_LEN]);
	 static const Theta_sens_type*
	 get_struct(uint8_t sensor_ID[SENSOR_ID_LEN]);
	 static const Theta_sens_type* get_struct(uint8_t index);
	 static const bool	 ID_is_same(uint8_t ID1[SENSOR_ID_LEN], uint8_t ID2[SENSOR_ID_LEN]);*/

#ifdef __x86_64

	static void dump (Theta_sens_type sens){
		cout << to_string(sens.sensorIdHash) << " ";
		cout << boost::format("%3.2f") % sens.minVal << " ";
		cout << boost::format("%3.2f") % sens.maxVal << " ";
		cout << sensorType2Str(sens.sensType) << " ";
		cout << to_string(sens.relayNr) << " ";
		string name(sens.shortname, SHORTNAME_LEN);
		cout << name << endl;
	}

#endif


private:

};

//@formatter:off
/*static constexpr ID_Table::Theta_sens_type theta_sensors_id_list[ID_Table::ID_TABLE_LEN] =
{
	{ 3103548024, -50,	50, 0, 0, 	'I','N','N','E','N', ' ', ' ', ' ', 0 },
	{ 1294211458,   2,	50, 0, 1, 	'W','S','T','_','O', ' ', ' ', ' ', 0 },
	{ 3932845497,   2,	50, 0, 1, 	'W','S','T','_','U', ' ', ' ', ' ', 0 },
	{ 3159888747,   2,	50, 0, 1,	'G','G','E','_','O', ' ', ' ', ' ', 0 },
	{ 3312979629,   1,	50, 0, 1,	'G','G','E','_','U', ' ', ' ', ' ', 0 },
	{ 1397168482, -50,	50, 0, 0,	'A','S','N','_','O', ' ', ' ', ' ', 0 },
	{ 3822322055, -50,	50, 0, 0,	'G','G','E','_','V', ' ', ' ', ' ', 0 },
	{ 4232230555,   1,	50, 0, 1,	'L','G','R','_','O', ' ', ' ', ' ', 0 },
	{ 3107815502,   1,	50, 0, 1,	'L','G','R','_','U', ' ', ' ', ' ', 0 },
	// nicht zugeteilt
	{ 3044662693, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 196920976 , -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 3504230533, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	// PCB-Modules
	{ 4058285955, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 2802107143, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 1186060932, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 4130136082, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 1431004260, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 3567186629, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 2575569828, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 668176713 , -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 1943981157, -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 },
	{ 26376452  , -50,	50, 0, 0, 	'U','N','K', 'N', 'O','W', 'N', ' ', 0 }
};*/
//@formatter:on

#endif /* THETASENSORS_ID_TABLE_H_ */

/*static constexpr theta_sens_type theta_sensors_id_list[ID_TABLE_LEN] =
 {
 // Sensortable of waterproof DS18B20 devices
 // ID (64 bit)								     minTemp RelayNo	Shortname (5 bytes)
 {  0x28, 0xFF, 0x89, 0x0E, 0x02, 0x17, 0x03, 0x4A,	-50,	0,    	'I','N','N','E','N', '\0'},
 {  0x28, 0xFF, 0x45, 0xB1, 0xC2, 0x17, 0x01, 0xAD,	2,		1,    	'W','S','T','_','O', '\0'},
 {  0x28, 0xFF, 0x03, 0xB7, 0xC2, 0x17, 0x01, 0xD1,	2,		1,    	'W','S','T','_','U', '\0'},
 {  0x28, 0xFF, 0x76, 0xC7, 0xC0, 0x17, 0x01, 0x89,	2,		1,		'G','G','E','_','O', '\0'},
 {  0x28, 0xFF, 0xAB, 0xB0, 0xC2, 0x17, 0x01, 0x5A,	1,		1, 		'G','G','E','_','U', '\0'},
 {  0x28, 0xFF, 0xC4, 0xAD, 0xC2, 0x17, 0x01, 0x81,	-50,	0, 		'A','S','N','_','O', '\0'},
 {  0x28, 0xFF, 0xA6, 0x40, 0xC0, 0x17, 0x02, 0x6A,	-50,	0, 		'G','G','E','_','V', '\0'},
 {  0x28, 0xFF, 0x34, 0x99, 0xC0, 0x17, 0x01, 0x2E,	 1,		1, 		'L','G','R','_','O', '\0'},
 {  0x28, 0xFF, 0x38, 0x17, 0x60, 0x17, 0x05, 0x54,	 1,		1, 		'L','G','R','_','U', '\0'},

 // nicht zugeteilt:
 {  0x28, 0xFF, 0x99, 0xC1, 0xC0, 0x17, 0x01, 0x35,	-50,	0, 		'U','N','K','N','O', '\0'},
 {  0x28, 0xFF, 0x10, 0xC4, 0xC0, 0x17, 0x01, 0x8C,	-50,	0, 		'U','N','K','N','O', '\0'},
 {  0x28, 0xFF, 0x75, 0x82, 0xC2, 0x17, 0x01, 0xE9,	-50,	0, 		'U','N','K','N','O', '\0'}

 // PCB-Modules
 // 28:1C:48:46:92:0A:02:3A
 // 28:E0:6F:45:92:13:02:4A
 // 28:61:4C:46:92:0A:02:F7
 // 28:7D:DA:45:92:0D:02:1C
 // 28:E0:3E:45:92:06:02:01
 // 28:8F:56:45:92:06:02:C9
 // 28:C4:0B:45:92:16:02:D1
 // 28:75:B3:45:92:0C:02:9B
 // 28:83:50:46:92:0A:02:ED
 // 28:0E:F4:45:92:0D:02:87
 };*/
