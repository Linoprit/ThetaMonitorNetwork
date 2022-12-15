/*
 * StartDisplayTask.cpp
 *
 *  Created on: 22.12.2018
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
#include <Application/Nokia_LCD/LCDFunctions.h>


void startDisplayTask(void * argument)
{
	UNUSED(argument);

	OsHelpers::delay(500); //older LCDs need more time to settle (2500)
	lcd::LCDFunctions::instance().init();
	lcd::LCDFunctions::instance().initHardware();

	for(;;) {
		// HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		lcd::LCDFunctions::instance().cycle();
		lcd::LCDFunctions::instance().incPage();
		OsHelpers::delay(2000);
	}
}




