/*
 * dspCallbackTask.cpp
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <main.h>
//#include <Instances/Common.h>
//#include <System/uart_printf.h>
#include <Tasks/nRF24Task.h>
#include <Tasks/measureTask.h>
//#include <Application/RadioLink/nRF24L01_Basis.h>
//#include <Application/RadioLink/Messages.h>
#include <Application/ThetaSensors/ID_Table.h>


/* original Code
#define TNSMT_REPEATS 5

ThetaMeasurement* remoteMsmt = NULL;
ThetaMeasurement* get_remoteMsmt(void) { return remoteMsmt; };

osPoolId  msg_pool;
osPoolId* get_msg_pool(void) { return &msg_pool; }

osMessageQId  queue;
osMessageQId* get_queue(void) {return &queue; }



void process_rx_data(Messages::nRF_dummy_struct* msg)
{
	if(msg->byte[0] == MSG_ID_THETA)
	{
		if (remoteMsmt != NULL)
		{
			//tx_printf("got ROM: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
			//		msg->byte[1], msg->byte[2], msg->byte[3], msg->byte[4],
			//		msg->byte[5], msg->byte[6], msg->byte[7], msg->byte[8]);

			float* theta = (float*) &msg->byte[9];
			remoteMsmt->update(&msg->byte[1], *theta);
		}
	}
}
*/


// C interface
void startnRF24Tsk(void const * argument)
{
	UNUSED(argument);

	for(;;) {
		osDelay(500);
	}

	/* original Code
	ID_Table::StationType stationType = Common::nRF24_basis->get_stationType();

	tx_printf("Init Radio Task, stationtype is: %i\n", stationType);

	Common::nRF24_basis->init();

	if (stationType == ID_Table::MASTER)
	{
		remoteMsmt = new ThetaMeasurement(); // no argument = Master mode

		osPoolDef(msg_pool, ID_TABLE_LEN, Messages::nRF_dummy_struct);
		osMessageQDef(queue, ID_TABLE_LEN, Messages::nRF_dummy_struct);

		msg_pool = osPoolCreate(osPool(msg_pool));
		queue    = osMessageCreate(osMessageQ(queue), NULL);

		for(;;)
		{
			//osDelay(1000);
			//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

			osEvent evt = osMessageGet(queue, NRF24_TASK_DELAY);// osWaitForever);
			if (evt.status == osEventTimeout)
				tx_printf("osMessageGet: timeout ");

			if (evt.status == osEventMessage)
			{
				Messages::nRF_dummy_struct* msg =
						(Messages::nRF_dummy_struct*) evt.value.p;
				process_rx_data(msg);
				osPoolFree(msg_pool, msg);
			}
		}
	}

	if (stationType != ID_Table::MASTER)
	{
		uint8_t i;

		ThetaMeasurement* msmnt = get_thetaMeasurement();
		while (msmnt == NULL)
		{
			tx_printf("NRF: Waiting for sensors to initialize... ");
			msmnt = get_thetaMeasurement();
			osDelay(1000);
		}
		tx_printf("OK.\n");

		for(;;)
		{
			//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			//NRF24L01::nRF24_TXResult tx_res;

			for (i=0; i < msmnt->get_sensorCount(); i++)
			{
				Messages::nRF_theta_struct msg;
				const SensorDataType* data = msmnt->get(i);

				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

				for(uint8_t i=0; i < 8; i++)
				{
					msg.sensor_id[i] = data->sensor_ID[i];
				}
				msg.theta	  = data->temperature;

				for (uint8_t j=0; j < TNSMT_REPEATS; j++)
				{
					Common::nRF24_basis->transmitPacket((uint8_t*) &msg);
					osDelay(200);
				}
			}

			osDelay(DISPLAY_TASK_DELAY);
		}
	}*/

}


