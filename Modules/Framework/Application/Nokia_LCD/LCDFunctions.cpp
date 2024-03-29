/*
 * LCDFunctions.cpp
 *
 *  Created on: 26.12.2018
 *      Author: harald
 */

#ifdef __x86_64
#include "stm32f1xx_hal.h"
#include <X86Tasks/SimulationTask.h>
#elif defined STM32F401xE
#include "stm32f4xx_hal.h"
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#endif

#include <Application/Nokia_LCD/LCDFunctions.h>
#include <Application/Sensors/SensorIdTable.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/Radio/RadioSlave.h>
#include <Config/config.h>
#include <System/Devices/PCD8544_LCD/PCD8544_basis.h>
#include <Libraries/HelpersLib.h>
#include <System/serialPrintf.h>
#include <array>
#include <string>
#include <cmath>

namespace lcd {

using namespace std;
using namespace snsrs;

// how long to keep the backlight on, after button-press
constexpr uint32_t TICKS_BCKLT_ON = 60000; // [ms]
// time, to keep the startup-screen, after init is done
constexpr uint32_t TICKS_KEEP_STATE_SCREEN = 4000; // [ms]

void LCDFunctions::init(void) {
	new (&instance()) LCDFunctions();
}

LCDFunctions& LCDFunctions::instance(void) {
	static LCDFunctions lcdFunctions;
	return lcdFunctions;
}

void LCDFunctions::initHardware(void) {
	_LCD_handle.initHardware();
	_LCD_handle.switch_font(FONT_5x8);
	_LCD_handle.clear();
	_tickLEDoff = HAL_GetTick() + TICKS_BCKLT_ON;
	_pages = static_cast<uint8_t>(ceil(
			((float) _sensorCount) / _LCD_handle.get_dispLines()));
	_tmpLineLen = _LCD_handle.get_chars_per_line() + 1;
	_tmpLine = static_cast<char*>(malloc(_tmpLineLen));
	clrTmpLine();

}

void LCDFunctions::printStates(void) {
	uint8_t act_line = 0;

	std::string line = "nRFState: ";
	line.append(
			radio::RadioSlave::instance().getNRF24L01_Basis()->getNRFStateStr());
	_LCD_handle.write_string(0, (_LCD_handle.line_2_y_pix(act_line)),
			line.c_str());
	act_line++;
	line.clear();

	line = snsrs::Sensors::instance().getNonVolatileData()->getStationTypeStr();
	line.append(" ID: ");
	line.append(std::to_string(
			snsrs::Sensors::instance().getNonVolatileData()->getStationId()));
	_LCD_handle.write_string(0, (_LCD_handle.line_2_y_pix(act_line)),
			line.c_str());
	act_line++;
	line.clear();

	line = "found DS1820: ";
	line.append(
			std::to_string(
					Sensors::instance().getThetaSensors()->getFoundDS1820()));
	_LCD_handle.write_string(0, (_LCD_handle.line_2_y_pix(act_line)),
			line.c_str());
	act_line++;

	_LCD_handle.display(); // push internal buffer to LCD
}

bool LCDFunctions::cycleInitScreen(void) {
	if (!Sensors::instance().getThetaSensors()->isInitDone()) {
		_tickLEDoff = OsHelpers::get_tick() + TICKS_BCKLT_ON;
		_holdStateTicks = OsHelpers::get_tick() + TICKS_KEEP_STATE_SCREEN;
	}
	if (OsHelpers::get_tick() < _holdStateTicks) {
		printStates();
		return true;
	}
	return false;
}

void LCDFunctions::checkBackgroundLight(void) {
	if (OsHelpers::get_tick() > _tickLEDoff) {
		_LCD_handle.backlight_off();
	}
}

void LCDFunctions::cycle(void) {
	// 6 lines, 16 chars
	uint8_t dispLines = _LCD_handle.get_dispLines();
	uint8_t start = _act_page * dispLines;
	uint8_t end = start + dispLines;
	uint8_t act_line = 0;

	if (cycleInitScreen() == true) {
		return;
	}

	checkBackgroundLight();
	_LCD_handle.clear(); // clear internal buffer

	_sensorCount =
			Sensors::instance().getThetaSensors()->getMeasurements()->getValidCount();
	_pages = static_cast<uint8_t>(ceil(
			((float) _sensorCount) / _LCD_handle.get_dispLines()));

	for (uint8_t i = start; i < end; i++) {
		if (i >= _sensorCount) {
			break;
		}
		clrTmpLine();

		if (osSemaphoreAcquire(localMsmntSemHandle, 20) != osOK) {
			return;
		}

		MeasurementType actSensor = _sensorMeasureArray->at(i);
		float actValue = actSensor.value;
		int32_t actHash = actSensor.sensorIdHash;
		osSemaphoreRelease(localMsmntSemHandle);

		NonVolatileData *nvData = Sensors::instance().getNonVolatileData();
		SensorIdTable::SensorIdType sensorConfig =
				_sensorIdTable->getSensorTableData(nvData, actHash);
		std::string shortname = std::string(sensorConfig.shortname, 8);
		if (sensorConfig.sensorIdHash == NonVolatileData::EMPTY_SENSOR_HASH) {
			shortname = "E2 inval";
		}

		copyString(_tmpLine, shortname.c_str(), shortname.length());
		pushTheta(actValue);

		if ((_pages > 1) && (act_line == 0)) { // display page-nr
			uint8_t pos = _LCD_handle.get_chars_per_line() - 2;
			HelpersLib::value2char(&_tmpLine[pos], 2, 0, _act_page);
		}

//		tx_printf("lcd: %i, %i, %i, %i\n", start, end, i,
//				_LCD_handle.line_2_y_pix(act_line));
//		tx_printf("  tmpline: '%s'  ", _tmpLine);
//		tx_printf("lcd: '%s'\n", shortname.c_str());

		_LCD_handle.write_string(0, (_LCD_handle.line_2_y_pix(act_line)),
				static_cast<char*>(_tmpLine));

		act_line++;
		OsHelpers::delay(10); // slow down, to avoid blocking E2 or display
	}
	_LCD_handle.display(); // push internal buffer to LCD
}

LCDFunctions::LCDFunctions() :
		_tickLEDoff { 0 }, _act_page { 0 }, _holdStateTicks { 0 } {
	_sensorMeasureArray =
			Sensors::instance().getThetaSensors()->getMeasurementArray();
	_sensorIdTable = Sensors::instance().getSensorIdTable();
}

void LCDFunctions::pushTheta(float theta) {
	std::string valueStr = HelpersLib::floatToStr(theta, 1u);
	if (!isnan(theta)) {
		_tmpLine[SensorIdTable::SHORTNAME_LEN] = ' ';
		copyString(&_tmpLine[SensorIdTable::SHORTNAME_LEN + 1],
				valueStr.c_str(), 5);
	} else {
		char txt[] = " --.-";
		copyString(&_tmpLine[SensorIdTable::SHORTNAME_LEN + 1], &txt[0], 5);
	}
}

void LCDFunctions::incPage(void) {
	_act_page++;
	if (_act_page >= _pages) {
		_act_page = 0;
	}
}

void LCDFunctions::clrTmpLine(void) {
	for (uint8_t i = 0; i < _tmpLineLen - 1; i++) {
		_tmpLine[i] = ' ';
	}
	_tmpLine[_tmpLineLen - 1] = '\0';
}

void LCDFunctions::copyString(char *tgt, const char *src, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		tgt[i] = src[i];
	}
}

void LCDFunctions::buttonPinCallback(void) {
	_tickLEDoff = OsHelpers::get_tick() + TICKS_BCKLT_ON;
	_LCD_handle.backlight_on();
	incPage();
}

} // namespace lcd

