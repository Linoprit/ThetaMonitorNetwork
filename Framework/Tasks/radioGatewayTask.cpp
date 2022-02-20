
/*
 * dspCallbackTask.cpp
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */
#ifdef __x86_64
#include <stm32f1xx.h>
#elif defined STM32F401xC ||  defined STM32F401xE
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
#include <Application/RadioGateway/RadioGateway.h>
#include <Application/RadioLink/RadioLink.h>
#include <Application/Sensors/ThetaSensors.h>
#include <System/serialPrintf.h>

#include "cmsis_os.h"
osThreadId_t gatewayTaskHandle;
gate::RadioGateway* radioGateway = nullptr;

void startGatewayTask(void *argument) {
	UNUSED(argument);

	for (;;) {
		radioGateway->cycle();
		OsHelpers::delay(500); // TODO must be '1'
	}
}

void initGatewayTask(void) {
	const osThreadAttr_t gatewayTask_attributes =
			{ .name = "gatewayTask", .stack_size = 128 * 4, .priority =
					(osPriority_t) osPriorityNormal, };

	gatewayTaskHandle = osThreadNew(startGatewayTask, NULL,
			&gatewayTask_attributes);

	radioGateway = new gate::RadioGateway();
}



