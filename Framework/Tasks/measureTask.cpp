/*
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#else
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
#endif
#include <System/OsHelpers.h>
#include "tasksDef.h"

void startMeasureTask(void *argument) {
	UNUSED(argument);

	for (;;) {
		OsHelpers::delay(500);
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	}

}