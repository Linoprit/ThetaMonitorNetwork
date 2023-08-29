
/*
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
#include "tasksDef.h"
#include <Config/config.h>
#include <Application/Sensors/Sensors.h>
#include <System/OsHelpers.h>

#ifdef __x86_64
void startMeasureTask(void *argument) {
	UNUSED(argument);
	for (;;) {
		OsHelpers::delay(1000);
	}
#else

void startMeasureTask(void *argument) {
	UNUSED(argument);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	OsHelpers::delay(2000);
	snsrs::Sensors::instance().init();
	snsrs::Sensors::instance().initHardware();

	for (;;) {
		//HAL_GPIO_TogglePin(DebLed_1_GPIO_Port, DebLed_1_Pin);

		snsrs::Sensors::instance().cycle();
		OsHelpers::delay(MEASURETASK_CYCLE);
	}
}

#endif // ifdef __x86_64
