/*
 * DS18B20.h
 *  Created on: Nov 1, 2021
 *      Author: harald
 * original code is from Ko-fi: https://github.com/nimaltd/ds18b20
 */
#ifndef	_DS18B20NEW_H
#define	_DS18B20NEW_H

// TODO replace with OsHelper version
#include "cmsis_os.h"
#define	Ds18b20Delay(x)			osDelay(x)

#include <stdbool.h>
#include "OneWire.h"

namespace oneWire {
class DS18B20 {
public:
	typedef struct {
		uint8_t Address[8];
		float Temperature;
		bool DataIsValid;
	} SensorType;

	/* Every onewire chip has different ROM code, but all the same chips has same family code */
	/* in case of DS18B20 this is 0x28 and this is first byte of ROM address */
	static constexpr uint8_t FAMILY_CODE = 0x28;
	static constexpr uint8_t CMD_ALARMSEARCH = 0xEC;

	/* DS18B20 read temperature command */
	static constexpr uint8_t CMD_CONVERTTEMP = 0x44; /* Convert temperature */
	static constexpr float DECIMAL_STEPS_12BIT = 0.0625;
	static constexpr float DECIMAL_STEPS_11BIT = 0.125;
	static constexpr float DECIMAL_STEPS_10BIT = 0.25;
	static constexpr float DECIMAL_STEPS_9BIT = 0.5;

	/* Bits locations for resolution */
	static constexpr uint8_t RESOLUTION_R1 = 6;
	static constexpr uint8_t RESOLUTION_R0 = 5;

	/* CRC enabled */
#ifdef DS18B20_USE_CRC	
static constexpr uint8_t DATA_LEN			=	9;
#else
	static constexpr uint8_t DATA_LEN = 2;
#endif

	typedef enum {
		Resolution_9bits = 9, /*!< DS18B20 9 bits resolution */
		Resolution_10bits = 10, /*!< DS18B20 10 bits resolution */
		Resolution_11bits = 11, /*!< DS18B20 11 bits resolution */
		Resolution_12bits = 12 /*!< DS18B20 12 bits resolution */
	} Resolution_t;

	DS18B20(OneWire *oneWire);
	~DS18B20() {
	}
	;

	uint8_t start(uint8_t *ROM);
	void startAll();
	bool read(uint8_t *ROM, float *destination);
	uint8_t getResolution(uint8_t *ROM);
	uint8_t setResolution(uint8_t *ROM, Resolution_t resolution);
	uint8_t isDs18b20(uint8_t *ROM);
	uint8_t setAlarmHighTemperature(uint8_t *ROM, int8_t temp);
	uint8_t setAlarmLowTemperature(uint8_t *ROM, int8_t temp);
	uint8_t disableAlarmTemperature(uint8_t *ROM);
	uint8_t alarmSearch();
	uint8_t allDone();

	uint8_t findAllSensors(void);
	bool searchFirstWithRetry(void);
	bool setAllResolution(Resolution_t resolution);
	bool doAllMeasure(void);
	SensorType* getAllSensors(void) {
		return _sensors;
	}
	;
	uint8_t getFoundSensors(void) {
		return _foundSensors;
	}

private:
	OneWire *_ow;
	SensorType _sensors[MAX_DEVICES_DS18B20];
	uint8_t _foundSensors;

};

} // namespace oneWire
#endif

