/*
 * MasterSerialTask.cpp
 *
 *  Created on: 02.02.2019
 *      Author: harald
 */

#include "stm32f1xx_hal.h"
#ifdef __x86_64
#include <X86Tasks/SimulationTask.h>
#else
#include "cmsis_os.h"
#include <main.h>
#endif

#include "tasksDef.h"
#include <Config/config.h>
#include <System/serialPrintf.h>
#include <System/CommandLine/CommandLine.h>
#include <System/OsHelpers.h>

// #include <Devices/Eeprom_at24c256/AT24Cxxx.h>
// #include <Instances/config.h>
// #include "../Application/ThetaSensors/ID_Table.h"
// #include "../Tasks/nRF24Task.h"
// #include <Tasks/masterSerialTask.h>
// #include <Sockets/CrcSocket.h>
//int tx_cycle();

void initCommandLine(void) {
	cLine::CommandLine::instance().init();
	OsHelpers::delay(500);
	cLine::CommandLine::instance().splash();
#ifndef __x86_64
	tx_cycle();
#endif
	OsHelpers::delay(500);
}

#ifdef __x86_64
void startMasterSerialTask(void) {
	for (;;) {
		cLine::CommandLine::instance().cycle();
		OsHelpers::delay(10);
	}
}
#else
void startMasterSerialTask(void *argument) {
	UNUSED(argument);

	initCommandLine();

	HAL_UART_RxCpltCallback (&SERIAL_UART); // init serial reception mechanism

	//
	// uint8_t addressTbl[][8] = {
	// 		{ 0x28, 0xFF, 0x89, 0x0E, 0x02, 0x17, 0x03, 0x4A }, { 0x28, 0xFF,
	// 				0x45, 0xB1, 0xC2, 0x17, 0x01, 0xAD } };
	// for (uint8_t i = 0; i < 2; i++) {
	// 	uint32_t result = CrcSocket::calc_chksum32(
	// 			reinterpret_cast<uint32_t*>(addressTbl[i]), 2);
	// 	printf("%u:\t %i %i %i %i %i %i %i %i\n", result, addressTbl[i][0],
	// 			addressTbl[i][1], addressTbl[i][2], addressTbl[i][3],
	// 			addressTbl[i][4], addressTbl[i][5], addressTbl[i][6],
	// 			addressTbl[i][7]);
	// }


	for (;;) {
		osDelay(20);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		//tx_printf("Message from hell\n");

		cLine::CommandLine::instance().cycle();
		tx_cycle();

	}
}
#endif

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

