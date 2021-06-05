/*
 * ID_Table.cpp
 *
 *  Created on: 11.11.2018
 *      Author: harald
 */

#include "ID_Table.h"
#include <stddef.h>


ID_Table::ID_Table()
{
	// NOP
}

const uint8_t ID_Table::get_stationNo(uint8_t sensor_ID[SENSOR_ID_LEN])
{
	for(uint8_t i=0; i < ID_TABLE_LEN; i++)
	{
		if(ID_is_same(sensor_ID, (uint8_t*) theta_sensors_id_list[i].sensor_ID) )
			return theta_sensors_id_list[i].minTemp;
	}
	return INVLD_TEMPERATURE;
}

const uint8_t	ID_Table::get_relayNo(uint8_t sensor_ID[SENSOR_ID_LEN])
{
	for(uint8_t i=0; i < ID_TABLE_LEN; i++)
	{
		if(ID_is_same(sensor_ID, (uint8_t*) theta_sensors_id_list[i].sensor_ID) )
			return theta_sensors_id_list[i].relayNo;
	}
	return INVLD_TEMPERATURE;
}

const theta_sens_type* ID_Table::get_struct(uint8_t sensor_ID[SENSOR_ID_LEN])
{
	for(uint8_t i=0; i < ID_TABLE_LEN; i++)
	{
		if(ID_is_same(sensor_ID, (uint8_t*) theta_sensors_id_list[i].sensor_ID) )
			return &theta_sensors_id_list[i];
	}
	return NULL;
}

const theta_sens_type* get_struct(uint8_t index)
{
	return &theta_sensors_id_list[index];
}

const char* ID_Table::get_shortname(uint8_t sensor_ID[SENSOR_ID_LEN])
{
	for(uint8_t i=0; i < ID_TABLE_LEN; i++)
	{
		if(ID_is_same(sensor_ID, (uint8_t*) theta_sensors_id_list[i].sensor_ID) )
			return theta_sensors_id_list[i].shortname;
	}
	return NULL;
}


const bool	ID_Table::ID_is_same(uint8_t ID1[SENSOR_ID_LEN], uint8_t ID2[SENSOR_ID_LEN])
{
	for (uint8_t i=0; i < SENSOR_ID_LEN; i++)
	{
		if(ID1[i] != ID2[i])
			return false;
	}
	return true;
}
