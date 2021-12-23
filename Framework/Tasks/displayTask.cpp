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
#include <Application/Nokia_LCD/LCDFunctions.h>

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
	OsHelpers::delay(2500); //older LCDs need time to settle
	lcd::LCDFunctions::instance().init();
	lcd::LCDFunctions::instance().initHardware();


	/* original Code
	osDelay(2000);
	lcd = new LCDFunctions();
	lcd->ISR_callback_fcn(); // Backlight on
*/
	// Backlight off
	//HAL_GPIO_WritePin(LCD_BCKLT_GPIO_Port, LCD_BCKLT_Pin, GPIO_PIN_SET);

	for(;;) {
		lcd::LCDFunctions::instance().cycle();
		lcd::LCDFunctions::instance().incPage();
		OsHelpers::delay(2000);

		// HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		/* original Code
		lcd->update();
		lcd->incPage();


		osDelay(DISPLAY_TASK_DELAY);
		*/
	}

}


