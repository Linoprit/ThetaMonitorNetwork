/*
 * MasterSerialTask.cpp
 *
 *  Created on: 02.02.2019
 *      Author: harald
 */

#include <Instances/config.h>
#include "../Application/ThetaSensors/ID_Table.h"
#include "../Tasks/nRF24Task.h"
#include <System/CommandLine/CommandLine.h>
#include <System/OsHelpers.h>
#include <Tasks/masterSerialTask.h>
#include <Devices/at24c256/at24c256/AT24C256.h>

#include <Sockets/CrcSocket.h>


void initCommandLine(void){
	cLine::CommandLine::instance().init();
	OsHelpers::delay(500);
	cLine::CommandLine::instance().splash();
	tx_cycle();
	OsHelpers::delay(500);
}

void startMasterSerialTask(void const *argument) {
	UNUSED(argument);

	initCommandLine();
	HAL_UART_RxCpltCallback(&SERIAL_UART); // init serial reception mechanism


	/* original Code
	 ThetaMeasurement* msmnt = NULL;
	 Messages::msg_nRF_theta_struct msg;
	 uint8_t i;

	 UART_TX_Socket* uart_tx_socket = new UART_TX_Socket(get_huart2());

	 while (msmnt == NULL)
	 {
	 tx_printf("masterSerial: Waiting for sensors to initialize... ");
	 msmnt       = get_remoteMsmt();

	 osDelay(1000);
	 }
	 tx_printf("OK.\n");
	 */


	uint8_t addressTbl[][8] = {
			{  0x28, 0xFF, 0x89, 0x0E, 0x02, 0x17, 0x03, 0x4A },
			{  0x28, 0xFF, 0x45, 0xB1, 0xC2, 0x17, 0x01, 0xAD }
	};
	for (uint8_t i=0; i < 2; i++ ) {
		uint32_t result = CrcSocket::calc_chksum32(
				reinterpret_cast<uint32_t*>(addressTbl[i]), 2);
		tx_printf("%u:\t %i %i %i %i %i %i %i %i\n", result,
				addressTbl[i][0],
				addressTbl[i][1],
				addressTbl[i][2],
				addressTbl[i][3],
				addressTbl[i][4],
				addressTbl[i][5],
				addressTbl[i][6],
				addressTbl[i][7]);
	}


	for (;;) {
		osDelay(100);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);






		cLine::CommandLine::instance().cycle();
		tx_cycle();



		/* original Code
		 osDelay(MASTERSERAL_TASK_DELAY);
		 //osDelay(3000);
		 //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		 for (i=0; i < ID_TABLE_LEN; i++)
		 {
		 const SensorDataType* const data = msmnt->get(i);

		 if (!ThetaMeasurement::isValid(txData->temperature))
		 continue;

		 Messages::prepare_uart_message(data, &msg);
		 //bool result =
		 uart_tx_socket->send_many_bytes(
		 (uint8_t*) &msg, sizeof(Messages::msg_nRF_theta_struct));

		 while(uart_tx_socket->is_TX_pending())
		 {
		 osDelay(1);
		 }
		 }
		 tx_printf("\n");
		 */
	}
}

/* original Code
 UART_TX_Socket::UART_TX_Socket (UART_HandleTypeDef *huart)
 {
 this->huart 	= huart;
 }

 bool UART_TX_Socket::send_byte(uint8_t byte)
 {
 return send_many_bytes(&byte, 1);
 }

 bool UART_TX_Socket::send_many_bytes(uint8_t *ptr_to_data, uint8_t len, uint16_t timeout)
 {
 uint16_t count = 0;
 HAL_UART_StateTypeDef uart_status = HAL_UART_GetState(huart);

 while ( ( (uart_status != HAL_UART_STATE_READY) &&
 (uart_status != HAL_UART_STATE_BUSY_RX) )  )
 {
 if(count >= timeout)
 {
 return ERROR;
 }

 osDelay(1);
 count++;
 uart_status = HAL_UART_GetState(huart);
 }

 HAL_UART_Transmit_DMA(huart, ptr_to_data, len);
 return SUCCESS;
 }

 bool UART_TX_Socket::is_TX_pending(void)
 {
 HAL_UART_StateTypeDef usart_status = HAL_UART_GetState(huart);

 return
 ( (usart_status == HAL_UART_STATE_BUSY_TX) ||
 (usart_status == HAL_UART_STATE_BUSY_TX_RX) );
 }
 */


/* EEprom Test Code
	uint8_t i=0;
	uint8_t txData[] = { 10U, 20U, 30U, 40U, 50U, 60U, 70U, 80U, 90U, 100U, 110U,
			120U, 130U, 140U, 150U, 160U };
	uint8_t rxData[] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
			0U, 0U, 0U, 0U, 0U };

	//AT24C256_Erase(Adr_AT24C256, 0U, 50U);
	//OsHelpers::delay(200);
	AT24C256_Write(Adr_AT24C256, 0U, &txData[0], 16U);
	OsHelpers::delay(200);
	AT24C256_Read(Adr_AT24C256, 0U, &rxData[0], 16U);
	OsHelpers::delay(200);


	uint32_t last = AT24C256_Search_Last(Adr_AT24C256, 10U, 50U);

	for(i=0; i < 16; i++) {
		if(rxData[i] != txData[i]) {
			tx_printf("unequal at %i: %i != %i\n", i, rxData[i], txData[i]);
			tx_cycle();
			OsHelpers::delay(100);
		} else {
			tx_printf("OK: %i\n", i);
		}
	}
	tx_printf("last: %lu\n", last);

	tx_cycle();
	OsHelpers::delay(10);
 */

