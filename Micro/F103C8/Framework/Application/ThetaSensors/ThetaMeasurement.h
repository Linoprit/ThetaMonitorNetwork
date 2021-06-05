/*
 * ThetaMeasurement.h
 *
 *  Created on: 20.12.2018
 *      Author: harald
 */

#ifndef THETASENSORS_THETAMEASUREMENT_H_
#define THETASENSORS_THETAMEASUREMENT_H_

#include "ID_Table.h"
#include "stm32f1xx_hal.h"


#define MAX_TIMEOUT 5 // measurement is invalid after X cycles

#define	 PACKED	__attribute__ ((packed))

typedef struct PACKED
{
	uint8_t  sensor_ID[SENSOR_ID_LEN];
	float 	 temperature;
	uint8_t  timeoutCount;
} SensorDataType;


class ThetaMeasurement {
public:

	ThetaMeasurement(uint8_t devices_count);
	ThetaMeasurement();
	virtual ~ThetaMeasurement() {};

	void put(uint8_t pos, uint8_t sensor_ID[SENSOR_ID_LEN], float temperature);
	uint8_t update(uint8_t sensor_ID[SENSOR_ID_LEN], float temperature);
	SensorDataType* get(uint8_t pos) const;
	uint8_t get_sensorCount(void) { return sensorCount; }
	void    incrementTimeouts(void);
	static bool    isValid(float temperature);

private:
	SensorDataType* sensorData;
	uint8_t sensorCount;
};

#endif /* THETASENSORS_THETAMEASUREMENT_H_ */
