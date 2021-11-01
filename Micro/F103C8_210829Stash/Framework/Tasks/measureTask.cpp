/*
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */


#include "measureTask.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
//#include "../Instances/Common.h"
//#include "../Framework/Devices/ds18b20/ow.h"
//#include "../Framework/Devices/ds18b20/ow_device_ds18x20.h"
//#include "../Framework/Devices/ds18b20/scan_devices.h"
//#include "../System/uart_printf.h"
//#include "../../Framework/Tasks/nRF24Task.h"


/* original Code

ow_t ow;
ow_rom_t rom_ids[20];
size_t sensorCount;
ThetaMeasurement* msmnt = NULL;

void _scanSensors(void)
{
	size_t i;

	owr_t result = owERR;
	while (result != owOK)
	{
		result = ow_search_devices(
				&ow, &rom_ids[0], sizeof(rom_ids) / sizeof(rom_ids[0]), &sensorCount);

		tx_printf("Sensors found: %i\n", sensorCount);

		// scan oneWire-bus
		for (i = 0; i < sensorCount; i++)
		{
			if (ow_ds18x20_is_b(&ow, &rom_ids[i])) {
				float temp;
				ow_ds18x20_set_resolution(&ow, &rom_ids[i], 12);
				if (ow_ds18x20_read(&ow, &rom_ids[i], &temp))
				{
					tx_printf("Device ROM addr: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
							rom_ids[i].rom[0], rom_ids[i].rom[1], rom_ids[i].rom[2], rom_ids[i].rom[3],
							rom_ids[i].rom[4], rom_ids[i].rom[5], rom_ids[i].rom[6], rom_ids[i].rom[7]);
				}
				else
				{
					tx_printf("TempSensor scan error.\n");
					osDelay(1000);
				}
			}
		}
	}
}

void _initMeasurment(void)
{
	size_t i;

	if (Common::nRF24_basis->get_stationType() == ID_Table::MASTER)
	{
		msmnt = get_remoteMsmt();
	}
	else
	{
		msmnt = new ThetaMeasurement(sensorCount);
		for (i = 0; i < sensorCount; i++)
		{
			msmnt->put(i, rom_ids[i].rom, (float) INVLD_TEMPERATURE);
		}
	}
}

void _relayOnOff(uint8_t relayNo, bool on)
{
	GPIO_PinState state;
	if (on == true)
		state = GPIO_PIN_RESET;
	else
		state = GPIO_PIN_SET;

	if (relayNo == 1)
		HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, state);

	if (relayNo == 2)
		HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, state);
}

void _checkChannel(uint8_t channel)
{
	size_t i;
	bool theta_on_reached 	  = false;
	bool theta_off_reached 	  = true;

	for (i = 0; i < sensorCount; i++)
	{
		SensorDataType* const sensData   = msmnt->get(i);
		const theta_sens_type* idTblData = ID_Table::get_struct(sensData->sensor_ID);

		int32_t	tmpTheta     = (sensData->temperature + 0.05) * 10;
		int32_t thres_on     = idTblData->minTemp * 10;
		int32_t thres_off    = (idTblData->minTemp + MIN_TEMP_HYSTERESIS) * 10;

		if ( (idTblData->relayNo != channel) ||
				(!ThetaMeasurement::isValid(sensData->temperature)) )
		{
			continue;
		}

		if (tmpTheta <= thres_on)		// one theta < on
			theta_on_reached = true;

		if (tmpTheta < thres_off) 		// one theta < off
			theta_off_reached = false;
	}

	if (theta_off_reached == true)
		_relayOnOff(channel, false);

	if (theta_on_reached == true)
		_relayOnOff(channel, true);
}


void _checkRelays(void)
{
	_checkChannel(1);
	_checkChannel(2);
}

*/



void startMeasureTsk(void const * argument)
{
	UNUSED(argument);

	/* original Code
	size_t i;

	tx_printf("Init Measure Task...\n");
	ow_init(&ow, NULL);    // Initialize 1-Wire library and set user argument to NULL

	osDelay(2000); // after power up, wait for sensors to wake
	_scanSensors();
	_initMeasurment();
	 */

	for(;;)
	{
		osDelay(500);

		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		//tx_printf("\nStart temperature conversion\n");

		/* original Code
		ow_ds18x20_start(&ow, NULL);  // Start conversion on all devices, use protected API
		osDelay(1000);

		// Read temperature on all devices
		for (i = 0; i < sensorCount; i++)
		{
			if (ow_ds18x20_is_b(&ow, &rom_ids[i]))
			{
				float temperature;
				//uint8_t resolution = ow_ds18x20_get_resolution(&ow, &rom_ids[i]);
				if (ow_ds18x20_read(&ow, &rom_ids[i], &temperature))
				{
					msmnt->update(rom_ids[i].rom, temperature);
				}
			}
		}

		_checkRelays();
		msmnt->incrementTimeouts();
		osDelay(MEASURE_TASK_DELAY);
		 */

	}

}

/* original Code
ThetaMeasurement* get_thetaMeasurement(void)
{
	return msmnt;
}
*/
