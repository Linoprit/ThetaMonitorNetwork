/*
 * dspCallbackTask.cpp
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#elif defined STM32F401xE
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#endif
#include <System/OsHelpers.h>
#include "tasksDef.h"
#include <Application/Radio/RadioSlave.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/RaspySerial/RaspySerial.h>
#include <System/serialPrintf.h>
#include <System/CommandLine/CommandLine.h>

extern void initGatewayTask(void);

void startnRF24Task(void *argument) {
	UNUSED(argument);
	bool isMaster =
			snsrs::Sensors::instance().getNonVolatileData()->getStationType()
					== snsrs::SensorIdTable::MASTER;

	if (isMaster) {
		radio::RadioMaster::instance().init();
		radio::RadioMaster::instance().initHardware();
		raspy::RaspySerial::instance().init();
	} else {
		radio::RadioSlave::instance().init();
		radio::RadioSlave::instance().initHardware();
	}

	for (;;) {
		if (isMaster) {
			radio::RadioMaster::instance().cycle();

			// TODO send internal Msmts in a cycle of STD_TX_CYCLE_TIME,
			// send remote Msmts when they come in
			raspy::RaspySerial::instance().cycle();

			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			OsHelpers::delay(1000); // TODO what is the exact cycletime?
		} else {
			OsHelpers::delay(1000);
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			radio::RadioSlave::instance().cycle();
			// TODO OsHelpers::delay(radio::RadioSlave::instance().getTransmitCycleTime());
		}
}

}

