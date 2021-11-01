/*
 * StartDisplayTask.cpp
 *
 *  Created on: 22.12.2018
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

//#include "../Instances/Common.h"
// #include "../Tasks/nRF24Task.h"
// #include "../Tasks/measureTask.h"
//#include "../Application/Nokia_LCD/LCDFunctions.h"


	/* original Code
LCDFunctions* lcd;

void clrTmpLine(char* tmpline, uint8_t len)
{
	for (uint8_t i=0; i < len; i++)
		tmpline[i] = ' ';
}*/

void startDisplayTask(void * argument)
{
	UNUSED(argument);

	/* original Code
	osDelay(2000); //older LCDs need time to settle
	lcd = new LCDFunctions();
	lcd->ISR_callback_fcn(); // Backlight on
*/

	for(;;) {
		OsHelpers::delay(500);

		/* original Code
		lcd->update();
		lcd->incPage();

		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		osDelay(DISPLAY_TASK_DELAY);
		*/
	}

}


