/*
 * nRF24L01_Basis.cpp
 *
 *  Created on: 05.11.2018
 *      Author: harald
 */

#include <Application/RadioLink/NRF24L01_Basis.h>
#include <System/OsHelpers.h>
#include <main.h>
#include "Messages.h"
#include <System/serialPrintf.h>
#include <Application/ThetaSensors/ThetaMeasurement.h>


NRF24L01_Basis::NRF24L01_Basis() :
		lostPkgCount { 0 }, retransCount { 0 } {
}

void NRF24L01_Basis::init() {
	nRF24.CE_L(); // RX/TX disabled

	msmnt::SensorIdTable::StationType stationType =
			msmnt::ThetaMeasurement::instance().getNonVolatileData().getStatType();
	tx_printf("Station Type is %u\n", static_cast<uint8_t>(stationType));

	// Configure the nRF24L01+
	tx_printf("nRF24L01+ checking... ");
	while (!nRF24.Check()) {
		OsHelpers::delay(10);
	}
	tx_printf(" OK\n");

	nRF24.Init(); // Initialize the nRF24L01 to its default state

	nRF24.SetRFChannel(nRF24_CHANNEL);
	nRF24.SetDataRate(nRF24_DR_250kbps); //  2Mbps);
	nRF24.SetCRCScheme(nRF24_CRC_2byte);
	nRF24.SetAddrWidth(5);

	const uint8_t nRF24_AA = nRF24_AA_OFF; // advanced shockburst
	if (stationType == msmnt::SensorIdTable::MASTER) {
		nRF24.SetAddr(nRF24_PIPE1, &nRF24_RX_ADDR1[0]);
		nRF24.SetRXPipe(nRF24_PIPE1, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.SetAddr(nRF24_PIPE2, &nRF24_RX_ADDR2[0]);
		nRF24.SetRXPipe(nRF24_PIPE2, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.SetAddr(nRF24_PIPE3, &nRF24_RX_ADDR3[0]);
		nRF24.SetRXPipe(nRF24_PIPE3, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.SetAddr(nRF24_PIPE4, &nRF24_RX_ADDR4[0]);
		nRF24.SetRXPipe(nRF24_PIPE4, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.SetAddr(nRF24_PIPE5, &nRF24_RX_ADDR5[0]);
		nRF24.SetRXPipe(nRF24_PIPE5, nRF24_AA, nRF_PAYLOAD_LEN);
	} else if (stationType == msmnt::SensorIdTable::SLAVE_01) {
		nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR1[0]); // program TX address
		nRF24.SetAddr(nRF24_PIPE0, &nRF24_TX_ADDR1[0]);
		nRF24.SetRXPipe(nRF24_PIPE1, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.EnableAA(1);
	} else if (stationType == msmnt::SensorIdTable::SLAVE_02) {
		nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR2[0]); // program TX address
		nRF24.SetAddr(nRF24_PIPE0, &nRF24_TX_ADDR2[0]);
		nRF24.SetRXPipe(nRF24_PIPE2, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.EnableAA(2);
	} else if (stationType == msmnt::SensorIdTable::SLAVE_03) {
		nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR3[0]); // program TX address
		nRF24.SetAddr(nRF24_PIPE0, &nRF24_TX_ADDR3[0]);
		nRF24.SetRXPipe(nRF24_PIPE3, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.EnableAA(3);
	} else if (stationType == msmnt::SensorIdTable::SLAVE_04) {
		nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR4[0]); // program TX address
		nRF24.SetAddr(nRF24_PIPE0, &nRF24_TX_ADDR4[0]);
		nRF24.SetRXPipe(nRF24_PIPE4, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.EnableAA(4);
	} else if (stationType == msmnt::SensorIdTable::SLAVE_05) {
		nRF24.SetAddr(nRF24_PIPETX, &nRF24_TX_ADDR5[0]); // program TX address
		nRF24.SetAddr(nRF24_PIPE0, &nRF24_TX_ADDR5[0]);
		nRF24.SetRXPipe(nRF24_PIPE5, nRF24_AA, nRF_PAYLOAD_LEN);
		nRF24.EnableAA(5);
	}

	//nRF24.DisableAA(0xFF); // Disable ShockBurst for all RX pipes
	nRF24.SetTXPower(nRF24_TXPWR_0dBm);
	nRF24.ClearIRQFlags();
	nRF24.SetPowerMode(nRF24_PWR_UP); // Wake the transceiver

	// if (stationType != msmnt::ID_Table::MASTER) {
	// 	nRF24.SetOperationalMode(nRF24_MODE_TX);
	// }
	// if (stationType == msmnt::ID_Table::MASTER) {
	// 	nRF24.SetOperationalMode(nRF24_MODE_RX);
	// 	nRF24.SetAddr(nRF24_PIPE1, &nRF24_RX_ADDR1[0]);
	// 	nRF24.SetRXPipe(nRF24_PIPE1, nRF24_AA_OFF, nRF_PAYLOAD_LEN);
	// 	nRF24.SetAddr(nRF24_PIPE2, &nRF24_RX_ADDR2[0]);
	// 	nRF24.SetRXPipe(nRF24_PIPE2, nRF24_AA_OFF, nRF_PAYLOAD_LEN);
	// 	nRF24.SetAddr(nRF24_PIPE3, &nRF24_RX_ADDR3[0]);
	// 	nRF24.SetRXPipe(nRF24_PIPE3, nRF24_AA_OFF, nRF_PAYLOAD_LEN);
	// 	nRF24.CE_H(); // enable Transmission
	// }

	tx_printf("Radio init done.\n");
}

NRF24L01::nRF24_TXResult NRF24L01_Basis::transmitPacket(uint8_t *pBuf,
		uint8_t length) {
	volatile int32_t wait = nRF24_WAIT_TIMEOUT;
	uint8_t otx;
	uint8_t otx_plos_cnt; // lost packet count
	uint8_t otx_arc_cnt;  // retransmit count
	uint8_t status;

	// Deassert the CE pin (in case if it still high)
	nRF24.CE_L();

	// Transfer data from the specified buffer to the TX FIFO
	nRF24.WritePayload(pBuf, length);

	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24.CE_H();

	// Poll the transceiver status register until one of the following flags will be set:
	//   TX_DS  - means the packet has been transmitted
	//   MAX_RT - means the maximum number of TX retransmits happened
	// note: this solution is far from perfect, better to use IRQ instead of polling the status
	do {
		status = nRF24.GetStatus();
		if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT )) {
			break;
		}
	} while (wait--);

	// Deassert the CE pin (Standby-II --> Standby-I)
	nRF24.CE_L();

	// Do statistics
	otx = nRF24.GetRetransmitCounters();
	otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT ) >> 4; // packets lost counter
	otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT ); // auto retransmissions counter
	add_stats(otx_plos_cnt, otx_arc_cnt);

	if (wait <= 0) {
		return NRF24L01::nRF24_TX_TIMEOUT;
	}

	nRF24.ClearIRQFlags();

	if (status & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		return NRF24L01::nRF24_TX_MAXRT;
	}

	if (status & nRF24_FLAG_TX_DS) {
		return NRF24L01::nRF24_TX_SUCCESS;
	}

	// Some banana happend, payload remaining in the TX FIFO, flush it
	nRF24.FlushTX();

	return NRF24L01::nRF24_TX_ERROR;
}

void NRF24L01_Basis::add_stats(uint8_t lostPkgCount, uint8_t retransCount) {
	this->lostPkgCount += lostPkgCount;
	this->retransCount += retransCount;
}

void NRF24L01_Basis::IRQ_Pin_callback(void) {
	// TODO if we want to receive statistics, we need the pipe-info
	//nRF24_RXResult pipe;
	//uint8_t payload_length;
	//uint8_t nRF24_payload[nRF_PAYLOAD_LEN];

	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	if (nRF24.GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
		// nRF24_RXResult pipe =
		//nRF24.ReadPayload(nRF24_payload, &payload_length);
		nRF24.ClearIRQFlags();

		// TODO
		// if (Common::nRF24_basis->get_stationType() == ID_Table::MASTER) {
		// 	osPoolId *msg_pool = get_msg_pool();
		// 	osMessageQId *queue = get_queue();
		//
		// 	if (payload_length != nRF_PAYLOAD_LEN)
		// 		return;
		//
		// 	Messages::nRF_dummy_struct *msg =
		// 			(Messages::nRF_dummy_struct*) osPoolAlloc(*msg_pool);
		//
		// 	for (uint8_t i = 0; i < sizeof(Messages::nRF_dummy_struct); i++) {
		// 		msg->byte[i] = nRF24_payload[i];
		// 	}
		//
		// 	osMessagePut(*queue, (uint32_t) msg, 0);
		// }

	}
}
