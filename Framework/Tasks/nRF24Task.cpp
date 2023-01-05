/*
 * dspCallbackTask.cpp
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
#include <System/OsHelpers.h>
#include "tasksDef.h"
#include <Application/Radio/RadioSlave.h>
#include <Application/Sensors/ThetaSensors.h>
#include <Application/Sensors/Sensors.h>
#include <System/serialPrintf.h>
#include <System/CommandLine/CommandLine.h>

#define PRODUCTIVE
//#define DEMO_RXTX_SINGLE
//#define DEMO_RXTX_SINGLE_AAON
//#define TEST_RXTX_BASIS_AAON

#ifdef TEST_RXTX_BASIS_AAON
void testRxBasisAAon(NRF24L01 *nRF24);
void testTxBasisAAon(NRF24L01 *nRF24);
#endif
#ifdef DEMO_RXTX_SINGLE_AAON
void demoTxSingleAAon(NRF24L01 *nRF24);
void demoRxSingleAAon(NRF24L01 *nRF24);
#endif
#ifdef DEMO_RXTX_SINGLE
void demoTxSingle(NRF24L01 *nRF24);
void demoRxSingle(NRF24L01 *nRF24);
#endif
#if defined TEST_RXTX_BASIS_AAON || defined DEMO_RXTX_SINGLE_AAON || defined DEMO_RXTX_SINGLE
NRF24L01* nRF24 = radio::RadioMaster::instance().getNRF24L01_Basis()->getNrf24();
#endif

void startnRF24Task(void *argument) {
	UNUSED(argument);

	bool isMaster =
			snsrs::Sensors::instance().getNonVolatileData()->getStationType()
					== snsrs::SensorIdTable::MASTER;

	OsHelpers::delay(500);

	if (isMaster) {
		radio::RadioMaster::instance().init();
		radio::RadioMaster::instance().initHardware();

#ifdef TEST_RXTX_BASIS_AAON
		testRxBasisAAon(nRF24);
#endif
#ifdef DEMO_RXTX_SINGLE_AAON
		demoRxSingleAAon(nRF24);
#endif
#ifdef DEMO_RXTX_SINGLE
		demoRxSingle(radio::RadioMaster::instance().getNRF24L01_Basis()->getNrf24());
#endif
	} else {
		radio::RadioSlave::instance().init();
		radio::RadioSlave::instance().initHardware();
#ifdef TEST_RXTX_BASIS_AAON
		testTxBasisAAon(nRF24);
#endif
#ifdef DEMO_RXTX_SINGLE_AAON
		demoTxSingleAAon(nRF24);
#endif
#ifdef DEMO_RXTX_SINGLE
		demoTxSingle(radio::RadioMaster::instance().getNRF24L01_Basis()->getNrf24());
#endif
	}

#ifdef PRODUCTIVE
	uint32_t lastTickSeconds = OsHelpers::get_tick_seconds();
	uint32_t delaySeconds = 10;

	for (;;) {
		if (isMaster) {
			//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

			uint32_t actTickSeconds = OsHelpers::get_tick_seconds();
			if(actTickSeconds > 120){
				delaySeconds = 300;
			}

			if (lastTickSeconds + delaySeconds < actTickSeconds){
				radio::RadioMaster::instance().cycleLocal();
				lastTickSeconds = actTickSeconds;
			}

			// avoid queue overflow
			radio::RadioMaster::instance().cycleRemote();
			OsHelpers::delay(500);

		} else {
			radio::RadioSlave::instance().cycle();

			if(OsHelpers::get_tick_seconds() < 120){
				OsHelpers::delay(10000);
			} else {
				OsHelpers::delay(radio::RadioSlave::instance().getTransmitCycleTime());
			}
		}
	}
#endif
}

// ------------------ test code ------------------
#if defined DEMO_RXTX_SINGLE_AAON || defined DEMO_RXTX_SINGLE || defined TEST_RXTX_BASIS_AAON
void print_payload(uint8_t *nRF24_payload, uint16_t payload_length) {
	for (uint_fast8_t i = 0; i < payload_length; i++) {
		tx_printf("%i ", nRF24_payload[i]);
	}
}

// Function to transmit data packet
// input:
//   pBuf - pointer to the buffer with data to transmit
//   length - length of the data buffer in bytes
// return: one of nRF24_TX_xx values
NRF24L01::nRF24_TXResult nRF24_TransmitPacket(NRF24L01 *nRF24, uint8_t *pBuf,
		uint8_t length) {
	volatile uint32_t wait = 0x000FFFFF;
	uint8_t status;

	nRF24->CE_L();
	nRF24->WritePayload(pBuf, length);
	nRF24->CE_H();

	// Poll the transceiver status register until one of the following flags will be set:
	//   TX_DS  - means the packet has been transmitted
	//   MAX_RT - means the maximum number of TX retransmits happened
	do {
		status = nRF24->GetStatus();
		if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT )) {
			break;
		}
	} while (wait--);

	// Standby-II --> Standby-I
	nRF24->CE_L();

	if (!wait) {
		return NRF24L01::nRF24_TX_TIMEOUT;
	}
	tx_printf("[%#02x] [%i] ", status, status);
	nRF24->ClearIRQFlags();

	if (status & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		return NRF24L01::nRF24_TX_MAXRT;
	}

	if (status & nRF24_FLAG_TX_DS) {
		return NRF24L01::nRF24_TX_SUCCESS;
	}

	// Some banana happens, a payload remains in the TX FIFO, flush it
	nRF24->FlushTX();
	return NRF24L01::nRF24_TX_ERROR;
}
#endif

#ifdef TEST_RXTX_BASIS_AAON
void testRxBasisAAon(NRF24L01 *_nRF24) {
	uint8_t nRF24_payload[32];
	nRF24_RXResult pipe;
	uint8_t payload_length;
	while (1) {
	//for(uint8_t i=0; i <= 10; i++){
		static uint8_t counter = 0;
		if (_nRF24->GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
			pipe = _nRF24->ReadPayloadDpl(nRF24_payload, &payload_length);
			_nRF24->ClearIRQFlags();

			tx_printf("RCV PIPE# %i ", pipe);
			tx_printf(" PAYLOAD:>");
			print_payload(nRF24_payload, payload_length);
			tx_printf("< lng=%i / %i \n",payload_length, counter++);
		} else {
			tx_printf("NOP %i\n", counter++);
		}
		OsHelpers::delay(300);
	}
}

void testTxBasisAAon(NRF24L01 *_nRF24) {
	uint8_t nRF24_payload[32];
	uint8_t payload_length = 10;// RADIO_MESSAGE_LEN;
	uint8_t i, j;
	NRF24L01::nRF24_TXResult txResult;
	uint8_t lostPkgCount;
	uint8_t retransCount;
	uint8_t rxBufferOverflows;
	NRF24L01_Basis *nRF_Basis =
			radio::RadioMaster::instance().getNRF24L01_Basis();

	j = 0;
	while (1) {
		for (i = 0; i < payload_length; i++) {
			nRF24_payload[i] = j++;
			if (j > 0x000000FF)
				j = 0;
		}
		tx_printf("PAYLOAD:>");
		print_payload(nRF24_payload, payload_length);
		tx_printf("< ... TX: ");

		txResult = nRF_Basis->transmitPacket(nRF24_payload, payload_length);
		lostPkgCount = nRF_Basis->get_lostPkgCount();
		retransCount = nRF_Basis->get_retransCount();
		rxBufferOverflows = nRF_Basis->get_rxBufferOverflows();

		switch (txResult) {
		case NRF24L01::nRF24_TX_SUCCESS:
			tx_printf("OK");
			break;
		case NRF24L01::nRF24_TX_TIMEOUT:
			tx_printf("TIMEOUT");
			break;
		case NRF24L01::nRF24_TX_ERROR:
		case NRF24L01::nRF24_TX_MAXRT:
			tx_printf("MAX RETRANSMIT %i / %i\n", _nRF24->GetStatus(),
					_nRF24->GetRetransmitCounters());
			nRF_Basis->resetStatistics();
			nRF_Basis->init(); // re-init
			break;

		case NRF24L01::nRF24_NOP:
			tx_printf("nRF24_NOP");
			break;
		case NRF24L01::nRF24_TX_IS_ONGOING:
			tx_printf("nRF24_TX_IS_ONGOING");
			break;
		case NRF24L01::nRF24_CHANNEL_SCAN_ACTIVE:
			tx_printf("nRF24_CHANNEL_SCAN_ACTIVE");
			break;
		default:
			tx_printf("ERROR");
			break;
		}
		tx_printf("\n");
		tx_printf(" ARC=%i / LOST=%i OVL=%i\n", retransCount, lostPkgCount,
				rxBufferOverflows);
		OsHelpers::delay(1000);
	}
}
#endif

#ifdef DEMO_RXTX_SINGLE
void demoRxSingle(NRF24L01 *nRF24) {
	uint8_t nRF24_payload[32];
	nRF24_RXResult pipe;
	uint8_t payload_length;

	// This is simple receiver with one RX pipe:
	//   - pipe#1 address: '0xE7 0x1C 0xE3'
	//   - payload: 10 bytes
	//   - RF channel: 115 (2515MHz)
	//   - data rate: 250kbps (minimum possible, to increase reception reliability)
	//   - CRC scheme: 2 byte
	// The transmitter sends packets to the address '0xE7 0x1C 0xE3'
	// without Auto-ACK (ShockBurst disabled)

	nRF24->DisableAA(0xFF); // Disable ShockBurst for all RX pipes
	nRF24->SetRFChannel(120);
	nRF24->SetDataRate(nRF24_DR_250kbps);
	nRF24->SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24->SetAddrWidth(3);

	// Configure RX PIPE#1
	static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
	nRF24->SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for RX pipe #1
	nRF24->SetRXPipe(nRF24_PIPE1, nRF24_AA_OFF, 10); // Auto-ACK: disabled, payload length: 10 bytes
	nRF24->SetOperationalMode(nRF24_MODE_RX);
	nRF24->SetPowerMode(nRF24_PWR_UP);
	nRF24->CE_H();

	while (1) {
		static uint8_t counter = 0;
		if (nRF24->GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
			pipe = nRF24->ReadPayload(nRF24_payload, &payload_length);
			nRF24->ClearIRQFlags();

			tx_printf("RCV PIPE# %i ", pipe);
			tx_printf(" PAYLOAD:>");
			print_payload(nRF24_payload, payload_length);
			tx_printf("< %i\n", counter++);
		} else {
			tx_printf("NOP %i\n", counter++);
		}
		OsHelpers::delay(300);
	}
}

void demoTxSingle(NRF24L01 *nRF24) {
	uint8_t nRF24_payload[32];
	uint32_t i, j;
	uint8_t payload_length;

	// This is simple transmitter (to one logic address):
	//   - TX address: '0xE7 0x1C 0xE3'
	//   - payload: 5 bytes
	//   - RF channel: 115 (2515MHz)
	//   - data rate: 250kbps (minimum possible, to increase reception reliability)
	//   - CRC scheme: 2 byte
	// The transmitter sends packets to the address
	// '0xE7 0x1C 0xE3' without Auto-ACK (ShockBurst disabled)

	nRF24->DisableAA(0xFF); // Disable ShockBurst for all RX pipes
	nRF24->SetRFChannel(120);
	nRF24->SetDataRate(nRF24_DR_250kbps);
	nRF24->SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24->SetAddrWidth(3);

	// Configure TX PIPE
	static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
	nRF24->SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
	nRF24->SetTXPower(nRF24_TXPWR_18dBm);
	nRF24->SetOperationalMode(nRF24_MODE_TX);
	nRF24->ClearIRQFlags();
	nRF24->SetPowerMode(nRF24_PWR_UP);

	j = 0;
	payload_length = 10;
	while (1) {
		for (i = 0; i < payload_length; i++) {
			nRF24_payload[i] = j++;
			if (j > 0x000000FF)
				j = 0;
		}

		tx_printf("PAYLOAD:>");
		print_payload(nRF24_payload, payload_length);
		tx_printf("< ... TX: ");

		NRF24L01::nRF24_TXResult tx_res = nRF24_TransmitPacket(nRF24,
				nRF24_payload, payload_length);
		switch (tx_res) {
		case NRF24L01::nRF24_TX_SUCCESS:
			tx_printf("OK");
			break;
		case NRF24L01::nRF24_TX_TIMEOUT:
			tx_printf("TIMEOUT");
			break;
		case NRF24L01::nRF24_TX_MAXRT:
			tx_printf("MAX RETRANSMIT %i / %i", nRF24->GetStatus(),
					nRF24->GetRetransmitCounters());

			// re-init
			nRF24->DisableAA(0xFF);
			nRF24->SetRFChannel(115);
			nRF24->SetDataRate(nRF24_DR_250kbps);
			nRF24->SetCRCScheme(nRF24_CRC_2byte);
			nRF24->SetAddrWidth(3);
			static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
			nRF24->SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
			nRF24->SetTXPower(nRF24_TXPWR_18dBm);
			nRF24->SetOperationalMode(nRF24_MODE_TX);
			nRF24->ClearIRQFlags();
			nRF24->SetPowerMode(nRF24_PWR_UP);

			break;
		default:
			tx_printf("ERROR");
			break;
		}
		tx_printf("\n");

		OsHelpers::delay(1000);
	}
}
#endif

#ifdef DEMO_RXTX_SINGLE_AAON
void demoRxSingleAAon(NRF24L01 *nRF24) {
	uint8_t nRF24_payload[32];
	nRF24_RXResult pipe;
	uint8_t payload_length;

	// This is simple receiver with Enhanced ShockBurst:
	//   - RX address: 'ESB'
	//   - payload: 10 bytes
	//   - RF channel: 40 (2440MHz)
	//   - data rate: 2Mbps
	//   - CRC scheme: 2 byte
	// The transmitter sends packets to the
	// address 'ESB' with Auto-ACK (ShockBurst enabled)
	nRF24->SetRFChannel(120);
	nRF24->SetDataRate(nRF24_DR_250kbps);
	nRF24->SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24->SetAddrWidth(3);

	// Configure RX PIPE
	static const uint8_t nRF24_ADDR[] = { 'E', 'S', 'B' };
	nRF24->SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for pipe
	nRF24->SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, 10); // Auto-ACK: enabled, payload length: 10 bytes

	nRF24->EnableAA(0xFF);  // for all pipes
	nRF24->SetTXPower(nRF24_TXPWR_18dBm);
	nRF24->SetOperationalMode(nRF24_MODE_RX);
	nRF24->ClearIRQFlags();
	nRF24->SetPowerMode(nRF24_PWR_UP);
	// Enable DPL
	nRF24->SetDynamicPayloadLength(nRF24_DPL_ON);
	nRF24->SetPayloadWithAck(1);

	nRF24->CE_H();

	while (1) {
		static uint8_t counter = 0;
		if (nRF24->GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
			pipe = nRF24->ReadPayload(nRF24_payload, &payload_length);
			nRF24->ClearIRQFlags();

			tx_printf("RCV PIPE# %i ", pipe);
			tx_printf(" PAYLOAD:>");
			print_payload(nRF24_payload, payload_length);
			tx_printf("< %i\n", counter++);
		} else {
			tx_printf("NOP %i\n", counter++);
		}
		OsHelpers::delay(300);
	}

}

void demoTxSingleAAon(NRF24L01 *nRF24) {
	uint8_t nRF24_payload[32];
	uint32_t i, j;
	uint8_t payload_length;

	// This is simple transmitter with Enhanced ShockBurst (to one logic address):
	//   - TX address: 'ESB'
	//   - payload: 10 bytes
	//   - RF channel: 40 (2440MHz)
	//   - data rate: 2Mbps
	//   - CRC scheme: 2 byte
	// The transmitter sends a 10-byte packets to the
	// address 'ESB' with Auto-ACK (ShockBurst enabled)
	nRF24->SetRFChannel(120);
	nRF24->SetDataRate(nRF24_DR_250kbps);
	nRF24->SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24->SetAddrWidth(3);

	// Configure TX PIPE
	static const uint8_t nRF24_ADDR[] = { 'E', 'S', 'B' };
	nRF24->SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
	nRF24->SetAddr(nRF24_PIPE0, nRF24_ADDR); // program address for pipe#0, must be same as TX (for Auto-ACK)
	nRF24->SetTXPower(nRF24_TXPWR_18dBm);
	nRF24->SetAutoRetr(nRF24_ARD_2500us, 10);
	nRF24->EnableAA(0xFF);
	//nRF24->EnableAA(nRF24_PIPE0);
	nRF24->SetOperationalMode(nRF24_MODE_TX);
	nRF24->ClearIRQFlags();
	nRF24->SetPowerMode(nRF24_PWR_UP);
	nRF24->SetDynamicPayloadLength(nRF24_DPL_ON);
	nRF24->SetPayloadWithAck(1);

	uint32_t packets_lost = 0; // global counter of lost packets
	uint8_t otx = 0;
	uint8_t otx_plos_cnt = 0; // lost packet count
	uint8_t otx_arc_cnt = 0; // retransmit count

	j = 0;
	payload_length = 10;
	while (1) {
		for (i = 0; i < payload_length; i++) {
			nRF24_payload[i] = j++;
			if (j > 0x000000FF)
				j = 0;
		}

		tx_printf("PAYLOAD:>");
		print_payload(nRF24_payload, payload_length);
		tx_printf("< ... TX: ");

		NRF24L01::nRF24_TXResult tx_res = nRF24_TransmitPacket(nRF24,
				nRF24_payload, payload_length);
		otx = nRF24->GetRetransmitCounters();
		otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT ) >> 4; // packets lost counter
		otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT ); // auto retransmissions counter

		switch (tx_res) {
		case NRF24L01::nRF24_TX_SUCCESS:
			tx_printf("OK");
			break;
		case NRF24L01::nRF24_TX_TIMEOUT:
			tx_printf("TIMEOUT");
			break;
		case NRF24L01::nRF24_TX_MAXRT:
			tx_printf("MAX RETRANSMIT %i / %i", nRF24->GetStatus(),
					nRF24->GetRetransmitCounters());
			packets_lost += otx_plos_cnt;
			nRF24->ResetPLOS();

			// re-init
			nRF24->SetRFChannel(115);
			nRF24->SetDataRate(nRF24_DR_250kbps);
			nRF24->SetCRCScheme(nRF24_CRC_2byte);
			nRF24->SetAddrWidth(3);
			static const uint8_t nRF24_ADDR[] = { 'E', 'S', 'B' };
			nRF24->SetAddr(nRF24_PIPETX, nRF24_ADDR);
			nRF24->SetAddr(nRF24_PIPE0, nRF24_ADDR);
			nRF24->SetTXPower(nRF24_TXPWR_18dBm);
			nRF24->SetAutoRetr(nRF24_ARD_2500us, 10);
			nRF24->EnableAA(0xFF);
			nRF24->SetOperationalMode(nRF24_MODE_TX);
			nRF24->ClearIRQFlags();
			nRF24->SetPowerMode(nRF24_PWR_UP);
			nRF24->SetDynamicPayloadLength(nRF24_DPL_ON);
			nRF24->SetPayloadWithAck(1);

			break;
		default:
			tx_printf("ERROR");
			break;
		}
		tx_printf("\n");
		tx_printf(" ARC=%i / LOST=%lu\n", otx_arc_cnt, packets_lost);

		OsHelpers::delay(2000);
	}
}
#endif

//void startnRF24Task(void *argument) {
//	UNUSED(argument);
//
//	bool isMaster =
//			snsrs::Sensors::instance().getNonVolatileData()->getStationType()
//					== snsrs::SensorIdTable::MASTER;
//
//	if (isMaster) {
//		radio::RadioMaster::instance().init();
//		radio::RadioMaster::instance().initHardware();
//		raspy::RaspySerial::instance().init();
//	} else {
//		radio::RadioSlave::instance().init();
//		radio::RadioSlave::instance().initHardware();
//	}
//
//
//	for (;;) {
//		// TODO clean up
//		HAL_GPIO_TogglePin(RELAY_2__GPIO_Port, RELAY_2__Pin);
//		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//		//tx_printf("nRFTask %i\n", counter++);
//
//		if (isMaster) {
//			radio::RadioMaster::instance().cycle();
//
//			// TODO send internal Msmts in a cycle of STD_TX_CYCLE_TIME,
//			// send remote Msmts when they come in
//			raspy::RaspySerial::instance().cycle();
//
//			//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//			OsHelpers::delay(1000); // TODO what is the exact cycletime?
//		} else {
//			radio::RadioSlave::instance().cycle();
//
//			// TODO remove
//			tx_printf("Lost Pkgs: %i | ",
//					radio::RadioSlave::instance().getNRF24L01_Basis()->get_lostPkgCount());
//
//			OsHelpers::delay(1000);
//			// TODO OsHelpers::delay(radio::RadioSlave::instance().getTransmitCycleTime());
//		}
//
//		// TODO remove
//		// OsHelpers::delay(500);
//		//radio::RadioSlave::instance().getNRF24L01_Basis()->getNrf24()->DumpConfig();
//	//	HAL_GPIO_WritePin(DebLed_1_GPIO_Port, DebLed_1_Pin, GPIO_PIN_RESET);
//	//	HAL_GPIO_WritePin(DebLed_2_GPIO_Port, DebLed_2_Pin, GPIO_PIN_RESET);
//	//	HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_RESET);
//	//	HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_RESET);
//	//	OsHelpers::delay(1000);
//
//}
//
//}

