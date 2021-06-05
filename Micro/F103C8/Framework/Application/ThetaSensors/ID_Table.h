/*
 * ID_Table.h
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#ifndef THETASENSORS_ID_TABLE_H_
#define THETASENSORS_ID_TABLE_H_

#include <stdint.h>


#define ID_TABLE_LEN  		  12
#define SENSOR_ID_LEN  		   8
#define SHORTNAME_LEN		   6
#define INVLD_TEMPERATURE	 255
#define MIN_TEMP_HYSTERESIS    1


#define	 PACKED	__attribute__ ((packed))

typedef struct PACKED
{
	uint8_t  sensor_ID[SENSOR_ID_LEN];
	int8_t   minTemp;
	uint8_t	 relayNo; // 0=no, 1=relay_1, 2=relay_2
	char     shortname[SHORTNAME_LEN];
} theta_sens_type;


static constexpr theta_sens_type theta_sensors_id_list[ID_TABLE_LEN] =
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
	/*  28:1C:48:46:92:0A:02:3A
		28:E0:6F:45:92:13:02:4A
		28:61:4C:46:92:0A:02:F7
		28:7D:DA:45:92:0D:02:1C
		28:E0:3E:45:92:06:02:01
		28:8F:56:45:92:06:02:C9
		28:C4:0B:45:92:16:02:D1
		28:75:B3:45:92:0C:02:9B
		28:83:50:46:92:0A:02:ED
		28:0E:F4:45:92:0D:02:87  */
};


class ID_Table {
public:
	enum StationType
	{
		MASTER 	 = 3,
		SLAVE_01 = 2,
		SLAVE_02 = 1,
		SLAVE_03 = 0
	};

	ID_Table();
	virtual ~ID_Table() {};

	static const uint8_t get_stationNo(uint8_t sensor_ID[SENSOR_ID_LEN]);
	static const uint8_t get_relayNo(uint8_t sensor_ID[SENSOR_ID_LEN]);
	static const char*   get_shortname(uint8_t sensor_ID[SENSOR_ID_LEN]);
	static const theta_sens_type*
						 get_struct(uint8_t sensor_ID[SENSOR_ID_LEN]);
	static const theta_sens_type* get_struct(uint8_t index);
	static const bool	 ID_is_same(uint8_t ID1[SENSOR_ID_LEN], uint8_t ID2[SENSOR_ID_LEN]);

private:

};

#endif /* THETASENSORS_ID_TABLE_H_ */
