/*
 * original code from: https://github.com/GolinskiyKonstantin/STM32_Lib_BME280_BMP180_BMP280
 */

#ifndef _BMPXX80_H
#define _BMPXX80_H

#include "math.h"

namespace boschBme280 {
class Bme280 {
public:

// Device Addresses (defined in config.h)
//#define BME280_I2CADDR	0xEC		// 0x76<<1	SDO -> GND
//#define BME280_I2CADDR	0xEE			// 0x77<<1	SDO -> VCC

//	Mode
// Huminidity oversampling
#define HUMINIDITY_DISABLE			0
#define HUMINIDITY_ULTRALOWPOWER		1
#define HUMINIDITY_LOWPOWER			2
#define HUMINIDITY_STANDARD			3
#define HUMINIDITY_HIGHRES			4
#define HUMINIDITY_ULTRAHIGH			5

// Temperature resolution
#define TEMPERATURE_DISABLE	0
#define TEMPERATURE_16BIT 	1
#define TEMPERATURE_17BIT 	2
#define TEMPERATURE_18BIT 	3
#define TEMPERATURE_19BIT 	4
#define TEMPERATURE_20BIT 	5

// Pressure oversampling
#define PRESSURE_DISABLE			0
#define PRESSURE_ULTRALOWPOWER	1
#define PRESSURE_LOWPOWER		2
#define PRESSURE_STANDARD		3
#define PRESSURE_HIGHRES			4
#define PRESSURE_ULTRAHIGHRES	5

// t_standby time
#define STANDBY_MS_0_5	0
#define STANDBY_MS_10	6
#define STANDBY_MS_20	7
#define STANDBY_MS_62_5	1
#define STANDBY_MS_125	2
#define STANDBY_MS_250 	3
#define STANDBY_MS_500	4
#define STANDBY_MS_1000	5

// IIR filter
#define FILTER_OFF	0
#define FILTER_X2 	1
#define FILTER_X4 	2
#define FILTER_X8	3
#define FILTER_X16 	4

// Mode
#define SLEEPMODE		0
#define FORCEDMODE		1
#define NORMALMODE		3

//	Coeffs registers
#define	DIG_T1		0x88
#define	DIG_T2		0x8A
#define	DIG_T3		0x8C

#define	DIG_P1		0x8E
#define	DIG_P2		0x90
#define	DIG_P3		0x92
#define	DIG_P4		0x94
#define	DIG_P5		0x96
#define	DIG_P6		0x98
#define	DIG_P7		0x9A
#define	DIG_P8		0x9C
#define	DIG_P9		0x9E

#define	DIG_H1		0xA1
#define	DIG_H2		0xE1
#define	DIG_H3		0xE3
#define	DIG_H4		0xE4
#define	DIG_H5		0xE5
#define	DIG_H6		0xE7

//	Registers
#define	CHIPID			0xD0
#define	VERSION			0xD1
#define	SOFTRESET		0xE0
#define	CAL26			0xE1  // R calibration stored in 0xE1-0xF0
#define	HUM_CONTROL		0xF2
#define	STATUS			0xF3
#define	CONTROL			0xF4
#define	CONFIG			0xF5
#define	PRESSUREDATA	0xF7
#define	TEMPDATA		0xFA
#define	HUMIDDATA		0xFD

//	Control bits
#define	MEASURING			(1<<3) // Conversion in progress

	void init(I2C_HandleTypeDef *i2c_handler, uint8_t temperature_resolution,
			uint8_t pressure_oversampling, uint8_t huminidity_oversampling,
			uint8_t mode);
	void setConfig(uint8_t standby_time, uint8_t filter);

	float readTemperature(void);
	int32_t readPressure(void);
	float readHuminidity(void);
	int8_t readTemperaturePressureHuminidity(float *temperature,
			int32_t *pressure, float *huminidity);
	float readAltitude(float sea_level_pa);

private:
	void delay_us(uint16_t us);
	uint8_t read8(uint8_t addr);
	uint16_t read16(uint8_t addr);
	uint16_t read16LE(uint8_t addr);
	void write8(uint8_t address, uint8_t data);
	uint32_t read24(uint8_t addr);
	uint8_t isReadingCalibration(void);

};

} //namespace bme280
#endif	/*	_BMPXX80_H */
