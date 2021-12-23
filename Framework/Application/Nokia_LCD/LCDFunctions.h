/*
 * LCDFunctions.h
 *
 *  Created on: 26.12.2018
 *      Author: harald
 */

#ifndef NOKIA_LCD_LCDFUNCTIONS_H_
#define NOKIA_LCD_LCDFUNCTIONS_H_

#include <Config/config.h>
#include <Devices/PCD8544_LCD/PCD8544_graphics.h>
#include <sys/_stdint.h>
#include <array>
#include <Application/ThetaSensors/ThetaMeasurement.h>
#include <Application/ThetaSensors/SensorIdTable.h>

namespace lcd {

using namespace std;
using namespace msmnt;

class LCDFunctions {
public:
	void init(void);
	static LCDFunctions& instance(void);
	void initHardware(void);
	void cycle(void);

	//
	void copyString(char *tgt, const char *src, uint8_t len);
	void clrTmpLine(void);
	void incPage(void);
	void buttonPinCallback(void);

private:
	PCD8544_graphics _LCD_handle;
	ThetaMeasurement::MeasurementArray *_sensorMeasureTable;
	SensorIdTable *_sensorIdTable;

	uint32_t _tickLEDoff;
	uint8_t _pages;
	char *_tmpLine;
	uint8_t _tmpLineLen;
	uint8_t _act_page;
	uint8_t _sensorCount;
	uint32_t _holdStateTicks;

	LCDFunctions();
	virtual ~LCDFunctions() {
	}
	bool cycleInitScreen(void);
	void checkBackgroundLight(void);
	void pushTheta(float theta);
	void printStates(void);
};

} // namespace lcd

#endif /* NOKIA_LCD_LCDFUNCTIONS_H_ */
