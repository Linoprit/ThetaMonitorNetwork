/*
 * MasterSerialTask.cpp
 *
 *  Created on: 02.02.2019
 *      Author: harald
 */

#ifdef __x86_64
#include "stm32f1xx_hal.h"
#include <X86Tasks/SimulationTask.h>
#elif defined STM32F401xE
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#endif

#include "tasksDef.h"
#include <Config/config.h>
#include <System/serialPrintf.h>
#include <System/CommandLine/CommandLine.h>
#include <Application/Sensors/Sensors.h>
#include <System/OsHelpers.h>
#include "cmsis_os.h"

// #include <Devices/Eeprom_at24c256/AT24Cxxx.h>
// #include <Instances/config.h>
// #include "../Application/ThetaSensors/ID_Table.h"
// #include "../Tasks/nRF24Task.h"
// #include <Tasks/masterSerialTask.h>
// #include <Sockets/CrcSocket.h>



void initCommandLine(void) {
	cLine::CommandLine::instance().init();
	OsHelpers::delay(500);
	cLine::CommandLine::instance().splash();

	bool isMaster =
			snsrs::Sensors::instance().getNonVolatileData()->getStationType()
					== snsrs::SensorIdTable::MASTER;
	uint32_t statID =
			snsrs::Sensors::instance().getNonVolatileData()->getStationId();
	std::string statTypeStr =
			snsrs::Sensors::instance().getNonVolatileData()->getStationTypeStr();

	if (isMaster) {
		tx_printf("Station is Master, ID is %lu, name: %s\n", statID, statTypeStr.c_str());
	} else {
		tx_printf("Station is Slave, ID is %lu, name: %s\n", statID, statTypeStr.c_str());
	}

#ifndef __x86_64
	tx_cycle();
#endif

	osTimerStart(ownSysTickTimerHandle, 1000U);
	OsHelpers::delay(500);
}

#ifdef __x86_64
void startMasterSerialTask(void) {
	for (;;) {
		cLine::CommandLine::instance().cycle();
		OsHelpers::delay(10);
	}
}
#else
void startMasterSerialTask(void *argument) {
	UNUSED(argument);

	initCommandLine();
	HAL_UART_RxCpltCallback(&SERIAL_UART); // init serial reception mechanism

	for (;;) {
		osDelay(20);

		//HAL_GPIO_TogglePin(DebLed_2_GPIO_Port, DebLed_2_Pin);
		// HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		cLine::CommandLine::instance().cycle();
		tx_cycle();
	}
}
#endif


