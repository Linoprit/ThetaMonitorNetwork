/*
 * nRF24L01.cpp
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */
#ifndef __x86_64
// FIXME make it working on X86

#include <System/Devices/nRF24L01/NRF24L01.h>
#include <Config/config.h>
#include <System/serialPrintf.h>
#include <System/OsHelpers.h>

using namespace std;

NRF24L01::NRF24L01() {
	spi_socket.setSpi(&nRF24_SPI);
}

uint8_t NRF24L01::LL_RW(uint8_t data) {
	return spi_socket.readwrite(data);
}

// Read a register
// input:
//   reg - number of register to read
// return: value of register
uint8_t NRF24L01::ReadReg(uint8_t reg) {
	uint8_t value;

	gpio_socket.nRF24_CSN_L();
	LL_RW(reg & nRF24_MASK_REG_MAP);
	value = LL_RW(nRF24_CMD_NOP);
	gpio_socket.nRF24_CSN_H();

	return value;
}

// Write a new value to register
// input:
//   reg - number of register to write
//   value - value to write
void NRF24L01::WriteReg(uint8_t reg, uint8_t value) {
	gpio_socket.nRF24_CSN_L();
	if (reg < nRF24_CMD_W_REGISTER) {
		// This is a register access
		LL_RW(nRF24_CMD_W_REGISTER | (reg & nRF24_MASK_REG_MAP ));
		LL_RW(value);
	} else {
		// This is a single byte command or future command/register
		LL_RW(reg);
		if ((reg != nRF24_CMD_FLUSH_TX ) && (reg != nRF24_CMD_FLUSH_RX )
				&& (reg != nRF24_CMD_REUSE_TX_PL ) && (reg != nRF24_CMD_NOP )) {
			// Send register value
			LL_RW(value);
		}
	}
	gpio_socket.nRF24_CSN_H();
}

// Read a multi-byte register
// input:
//   reg - number of register to read
//   pBuf - pointer to the buffer for register data
//   count - number of bytes to read
void NRF24L01::ReadMBReg(uint8_t reg, uint8_t *pBuf, uint8_t count) {
	gpio_socket.nRF24_CSN_L();
	LL_RW(reg);
	while (count--) {
		*pBuf++ = LL_RW(nRF24_CMD_NOP);
	}
	gpio_socket.nRF24_CSN_H();
}

// Write a multi-byte register
// input:
//   reg - number of register to write
//   pBuf - pointer to the buffer with data to write
//   count - number of bytes to write
void NRF24L01::WriteMBReg(uint8_t reg, uint8_t *pBuf, uint8_t count) {
	gpio_socket.nRF24_CSN_L();
	LL_RW(reg);
	while (count--) {
		LL_RW(*pBuf++);
	}
	gpio_socket.nRF24_CSN_H();
}

// Set transceiver to it's initial state
// note: RX/TX pipe addresses remains untouched
void NRF24L01::Init(void) {
	// Write to registers their initial values
	WriteReg(nRF24_REG_CONFIG, 0x08);
	WriteReg(nRF24_REG_EN_AA, 0x3F);
	WriteReg(nRF24_REG_EN_RXADDR, 0x03);
	WriteReg(nRF24_REG_SETUP_AW, 0x03);
	WriteReg(nRF24_REG_SETUP_RETR, 0x03);
	WriteReg(nRF24_REG_RF_CH, 0x02);
	WriteReg(nRF24_REG_RF_SETUP, 0x0E);
	WriteReg(nRF24_REG_STATUS, 0x00);
	WriteReg(nRF24_REG_RX_PW_P0, 0x00);
	WriteReg(nRF24_REG_RX_PW_P1, 0x00);
	WriteReg(nRF24_REG_RX_PW_P2, 0x00);
	WriteReg(nRF24_REG_RX_PW_P3, 0x00);
	WriteReg(nRF24_REG_RX_PW_P4, 0x00);
	WriteReg(nRF24_REG_RX_PW_P5, 0x00);
	WriteReg(nRF24_REG_DYNPD, 0x00);
	WriteReg(nRF24_REG_FEATURE, 0x00);

	// Clear the FIFO's
	FlushRX();
	FlushTX();

	// Clear any pending interrupt flags
	ClearIRQFlags();

	// Deassert CSN pin (chip release)
	gpio_socket.nRF24_CSN_H();
}

// Check if the nRF24L01 present
// return:
//   1 - nRF24L01 is online and responding
//   0 - received sequence differs from original
uint8_t NRF24L01::Check(void) {
	uint8_t rxbuf[5];
	uint8_t i;
	uint8_t *ptr = (uint8_t*) nRF24_TEST_ADDR;

	// Write test TX address and read TX_ADDR register
	WriteMBReg(nRF24_CMD_W_REGISTER | nRF24_REG_TX_ADDR, ptr, 5);
	ReadMBReg(nRF24_CMD_R_REGISTER | nRF24_REG_TX_ADDR, rxbuf, 5);

	// Compare buffers, return error on first mismatch
	for (i = 0; i < 5; i++) {
		if (rxbuf[i] != *ptr++)
			return 0;
	}

	return 1;
}

// Control transceiver power mode
// input:
//   mode - new state of power mode, one of nRF24_PWR_xx values
void NRF24L01::SetPowerMode(uint8_t mode) {
	uint8_t reg;

	reg = ReadReg(nRF24_REG_CONFIG);
	if (mode == nRF24_PWR_UP) {
		// Set the PWR_UP bit of CONFIG register to wake the transceiver
		// It goes into Stanby-I mode with consumption about 26uA
		reg |= nRF24_CONFIG_PWR_UP;
	} else {
		// Clear the PWR_UP bit of CONFIG register to put the transceiver
		// into power down mode with consumption about 900nA
		reg &= ~nRF24_CONFIG_PWR_UP;
	}
	WriteReg(nRF24_REG_CONFIG, reg);
}

// Set transceiver operational mode
// input:
//   mode - operational mode, one of nRF24_MODE_xx values
void NRF24L01::SetOperationalMode(uint8_t mode) {
	uint8_t reg;

	// Configure PRIM_RX bit of the CONFIG register
	reg = ReadReg(nRF24_REG_CONFIG);
	reg &= ~nRF24_CONFIG_PRIM_RX;
	reg |= (mode & nRF24_CONFIG_PRIM_RX );
	WriteReg(nRF24_REG_CONFIG, reg);
}

// Set transceiver DynamicPayloadLength feature for all the pipes
// input:
//   mode - status, one of nRF24_DPL_xx values
void NRF24L01::SetDynamicPayloadLength(uint8_t mode) {
	uint8_t reg;
	reg = ReadReg(nRF24_REG_FEATURE);
	if (mode) {
		WriteReg(nRF24_REG_FEATURE, reg | nRF24_FEATURE_EN_DPL);
		WriteReg(nRF24_REG_DYNPD, 0x1F);
	} else {
		WriteReg(nRF24_REG_FEATURE, reg & ~ nRF24_FEATURE_EN_DPL);
		WriteReg(nRF24_REG_DYNPD, 0x0);
	}
}

// Enables Payload With Ack. NB Refer to the datasheet for proper retransmit timing.
// input:
//   mode - status, 1 or 0
void NRF24L01::SetPayloadWithAck(uint8_t mode) {
	uint8_t reg;
	reg = ReadReg(nRF24_REG_FEATURE);
	if (mode) {
		WriteReg(nRF24_REG_FEATURE, reg | nRF24_FEATURE_EN_ACK_PAY);
	} else {
		WriteReg(nRF24_REG_FEATURE, reg & ~ nRF24_FEATURE_EN_ACK_PAY);
	}
}

// Configure transceiver CRC scheme
// input:
//   scheme - CRC scheme, one of nRF24_CRC_xx values
// note: transceiver will forcibly turn on the CRC in case if auto acknowledgment
//       enabled for at least one RX pipe
void NRF24L01::SetCRCScheme(uint8_t scheme) {
	uint8_t reg;

	// Configure EN_CRC[3] and CRCO[2] bits of the CONFIG register
	reg = ReadReg(nRF24_REG_CONFIG);
	reg &= ~nRF24_MASK_CRC;
	reg |= (scheme & nRF24_MASK_CRC );
	WriteReg(nRF24_REG_CONFIG, reg);
}

// Set frequency channel
// input:
//   channel - radio frequency channel, value from 0 to 127
// note: frequency will be (2400 + channel)MHz
// note: PLOS_CNT[7:4] bits of the OBSERVER_TX register will be reset
void NRF24L01::SetRFChannel(uint8_t channel) {
	WriteReg(nRF24_REG_RF_CH, channel);
}

// Set automatic retransmission parameters
// input:
//   ard - auto retransmit delay, one of nRF24_ARD_xx values
//   arc - count of auto retransmits, value form 0 to 15
// note: zero arc value means that the automatic retransmission disabled
void NRF24L01::SetAutoRetr(uint8_t ard, uint8_t arc) {
	// Set auto retransmit settings (SETUP_RETR register)
	WriteReg(nRF24_REG_SETUP_RETR,
			(uint8_t) ((ard << 4) | (arc & nRF24_MASK_RETR_ARC )));
}

// Set of address widths
// input:
//   addr_width - RX/TX address field width, value from 3 to 5
// note: this setting is common for all pipes
void NRF24L01::SetAddrWidth(uint8_t addr_width) {
	WriteReg(nRF24_REG_SETUP_AW, addr_width - 2);
}

// Set static RX address for a specified pipe
// input:
//   pipe - pipe to configure address, one of nRF24_PIPEx values
//   addr - pointer to the buffer with address
// note: pipe can be a number from 0 to 5 (RX pipes) and 6 (TX pipe)
// note: buffer length must be equal to current address width of transceiver
// note: for pipes[2..5] only first byte of address will be written because
//       other bytes of address equals to pipe1
// note: for pipes[2..5] only first byte of address will be written because
//       pipes 1-5 share the four most significant address bytes
void NRF24L01::SetAddr(uint8_t pipe, const uint8_t *addr) {
	uint8_t addr_width;

	// RX_ADDR_Px register
	switch (pipe) {
	case nRF24_PIPETX:
	case nRF24_PIPE0:
	case nRF24_PIPE1:
		// Get address width
		addr_width = ReadReg(nRF24_REG_SETUP_AW) + 1;
		// Write address in reverse order (LSByte first)
		addr += addr_width;
		gpio_socket.nRF24_CSN_L();
		LL_RW(nRF24_CMD_W_REGISTER | nRF24_ADDR_REGS[pipe]);
		do {
			LL_RW(*addr--);
		} while (addr_width--);
		gpio_socket.nRF24_CSN_H();
		break;
	case nRF24_PIPE2:
	case nRF24_PIPE3:
	case nRF24_PIPE4:
	case nRF24_PIPE5:
		// Write address LSBbyte (only first byte from the addr buffer)
		WriteReg(nRF24_ADDR_REGS[pipe], *addr);
		break;
	default:
		// Incorrect pipe number -> do nothing
		break;
	}
}

// Configure RF output power in TX mode
// input:
//   tx_pwr - RF output power, one of nRF24_TXPWR_xx values
void NRF24L01::SetTXPower(uint8_t tx_pwr) {
	uint8_t reg;

	// Configure RF_PWR[2:1] bits of the RF_SETUP register
	reg = ReadReg(nRF24_REG_RF_SETUP);
	reg &= ~nRF24_MASK_RF_PWR;
	reg |= tx_pwr;
	WriteReg(nRF24_REG_RF_SETUP, reg);
}

// Configure transceiver data rate
// input:
//   data_rate - data rate, one of nRF24_DR_xx values
void NRF24L01::SetDataRate(uint8_t data_rate) {
	uint8_t reg;

	// Configure RF_DR_LOW[5] and RF_DR_HIGH[3] bits of the RF_SETUP register
	reg = ReadReg(nRF24_REG_RF_SETUP);
	reg &= ~nRF24_MASK_DATARATE;
	reg |= data_rate;
	WriteReg(nRF24_REG_RF_SETUP, reg);
}

// Configure a specified RX pipe
// input:
//   pipe - number of the RX pipe, value from 0 to 5
//   aa_state - state of auto acknowledgment, one of nRF24_AA_xx values
//   payload_len - payload length in bytes
void NRF24L01::SetRXPipe(uint8_t pipe, uint8_t aa_state, uint8_t payload_len) {
	uint8_t reg;

	// Enable the specified pipe (EN_RXADDR register)
	reg = (ReadReg(nRF24_REG_EN_RXADDR) | (1 << pipe)) & nRF24_MASK_EN_RX;
	WriteReg(nRF24_REG_EN_RXADDR, reg);

	// Set RX payload length (RX_PW_Px register)
	WriteReg(nRF24_RX_PW_PIPE[pipe], payload_len & nRF24_MASK_RX_PW);

	// Set auto acknowledgment for a specified pipe (EN_AA register)
	reg = ReadReg(nRF24_REG_EN_AA);
	if (aa_state == nRF24_AA_ON) {
		reg |= (1 << pipe);
	} else {
		reg &= ~(1 << pipe);
	}
	WriteReg(nRF24_REG_EN_AA, reg);
}

// Disable specified RX pipe
// input:
//   PIPE - number of RX pipe, value from 0 to 5
void NRF24L01::ClosePipe(uint8_t pipe) {
	uint8_t reg;

	reg = ReadReg(nRF24_REG_EN_RXADDR);
	reg &= ~(1 << pipe);
	reg &= nRF24_MASK_EN_RX;
	WriteReg(nRF24_REG_EN_RXADDR, reg);
}

// Enable the auto retransmit (a.k.a. enhanced ShockBurst) for the specified RX pipe
// input:
//   pipe - number of the RX pipe, value from 0 to 5
void NRF24L01::EnableAA(uint8_t pipe) {
	uint8_t reg;

	if (pipe > 5) {
		// Disable Auto-ACK for ALL pipes
		WriteReg(nRF24_REG_EN_AA, 0x3F);
	} else {
		// Set bit in EN_AA register
		reg = ReadReg(nRF24_REG_EN_AA);
		reg |= (1 << pipe);
		WriteReg(nRF24_REG_EN_AA, reg);
	}
}

// Disable the auto retransmit (a.k.a. enhanced ShockBurst) for one or all RX pipes
// input:
//   pipe - number of the RX pipe, value from 0 to 5, any other value will disable AA for all RX pipes
void NRF24L01::DisableAA(uint8_t pipe) {
	uint8_t reg;

	if (pipe > 5) {
		// Disable Auto-ACK for ALL pipes
		WriteReg(nRF24_REG_EN_AA, 0x00);
	} else {
		// Clear bit in the EN_AA register
		reg = ReadReg(nRF24_REG_EN_AA);
		reg &= ~(1 << pipe);
		WriteReg(nRF24_REG_EN_AA, reg);
	}
}

// Get value of the STATUS register
// return: value of STATUS register
uint8_t NRF24L01::GetStatus(void) {
	return ReadReg(nRF24_REG_STATUS);
}

// Get pending IRQ flags
// return: current status of RX_DR, TX_DS and MAX_RT bits of the STATUS register
uint8_t NRF24L01::GetIRQFlags(void) {
	return (ReadReg(nRF24_REG_STATUS) & nRF24_MASK_STATUS_IRQ );
}

// Get status of the RX FIFO
// return: one of the nRF24_STATUS_RXFIFO_xx values
uint8_t NRF24L01::GetStatus_RXFIFO(void) {
	return (ReadReg(nRF24_REG_FIFO_STATUS) & nRF24_MASK_RXFIFO );
}

// Get status of the TX FIFO
// return: one of the nRF24_STATUS_TXFIFO_xx values
// note: the TX_REUSE bit ignored
uint8_t NRF24L01::GetStatus_TXFIFO(void) {
	return ((ReadReg(nRF24_REG_FIFO_STATUS) & nRF24_MASK_TXFIFO ) >> 4);
}

// Get pipe number for the payload available for reading from RX FIFO
// return: pipe number or 0x07 if the RX FIFO is empty
uint8_t NRF24L01::GetRXSource(void) {
	return ((ReadReg(nRF24_REG_STATUS) & nRF24_MASK_RX_P_NO ) >> 1);
}

// Get auto retransmit statistic
// return: value of OBSERVE_TX register which contains two counters encoded in nibbles:
//   high - lost packets count (max value 15, can be reseted by write to RF_CH register)
//   low  - retransmitted packets count (max value 15, reseted when new transmission starts)
uint8_t NRF24L01::GetRetransmitCounters(void) {
	return (ReadReg(nRF24_REG_OBSERVE_TX));
}

// Reset packet lost counter (PLOS_CNT bits in OBSERVER_TX register)
void NRF24L01::ResetPLOS(void) {
	uint8_t reg;

	// The PLOS counter is reset after write to RF_CH register
	reg = ReadReg(nRF24_REG_RF_CH);
	WriteReg(nRF24_REG_RF_CH, reg);
}

// Flush the TX FIFO
void NRF24L01::FlushTX(void) {
	WriteReg(nRF24_CMD_FLUSH_TX, nRF24_CMD_NOP);
}

// Flush the RX FIFO
void NRF24L01::FlushRX(void) {
	WriteReg(nRF24_CMD_FLUSH_RX, nRF24_CMD_NOP);
}

// Clear any pending IRQ flags
void NRF24L01::ClearIRQFlags(void) {
	uint8_t reg;

	// Clear RX_DR, TX_DS and MAX_RT bits of the STATUS register
	reg = ReadReg(nRF24_REG_STATUS);
	reg |= nRF24_MASK_STATUS_IRQ;
	WriteReg(nRF24_REG_STATUS, reg);
}

// Write TX payload
// input:
//   pBuf - pointer to the buffer with payload data
//   length - payload length in bytes
void NRF24L01::WritePayload(uint8_t *pBuf, uint8_t length) {
	WriteMBReg(nRF24_CMD_W_TX_PAYLOAD, pBuf, length);
}

uint8_t NRF24L01::GetRxDplPayloadWidth(void) {
	uint8_t value;

	CSN_L();
	LL_RW(nRF24_CMD_R_RX_PL_WID);
	value = LL_RW(nRF24_CMD_NOP);
	CSN_H();

	return value;
}

nRF24_RXResult NRF24L01::ReadPayloadGeneric(uint8_t *pBuf,
		uint8_t *length, uint8_t dpl) {
	uint8_t pipe;

	// Extract a payload pipe number from the STATUS register
	pipe = (ReadReg(nRF24_REG_STATUS) & nRF24_MASK_RX_P_NO ) >> 1;

	// RX FIFO empty?
	if (pipe < 6) {
		// Get payload length
		if (dpl) {
			*length = GetRxDplPayloadWidth();
			if (*length > 32) { //broken packet
				*length = 0;
				FlushRX();
			}
		} else {
			*length = ReadReg(nRF24_RX_PW_PIPE[pipe]);
		}

		// Read a payload from the RX FIFO
		if (*length) {
			ReadMBReg(nRF24_CMD_R_RX_PAYLOAD, pBuf, *length);
		}
		return ((nRF24_RXResult) pipe);
	}
	// The RX FIFO is empty
	*length = 0;

	return nRF24_RX_EMPTY;
}

// Read top level payload available in the RX FIFO
// input:
//   pBuf - pointer to the buffer to store a payload data
//   length - pointer to variable to store a payload length
// return: one of nRF24_RX_xx values
//   nRF24_RX_PIPEX - packet has been received from the pipe number X
//   nRF24_RX_EMPTY - the RX FIFO is empty
nRF24_RXResult NRF24L01::ReadPayload(uint8_t *pBuf, uint8_t *length) {
	return ReadPayloadGeneric(pBuf, length, 0);
}

nRF24_RXResult NRF24L01::ReadPayloadDpl(uint8_t *pBuf, uint8_t *length) {
	return ReadPayloadGeneric(pBuf, length, 1);
}

std::string NRF24L01::txResultToStr(nRF24_TXResult errorCode) {
	if (errorCode == NRF24L01::nRF24_TX_ERROR) {
		return string("nRF24_TX_ERROR");
	} else if (errorCode == NRF24L01::nRF24_TX_SUCCESS) {
		return string("nRF24_TX_SUCCESS");
	} else if (errorCode == NRF24L01::nRF24_TX_TIMEOUT) {
		return string("nRF24_TX_TIMEOUT");
	} else if (errorCode == NRF24L01::nRF24_TX_MAXRT) {
		return string("nRF24_TX_MAXRT");
	}
	return string("nRF24_TX_UNKNOWN");
}

// Print nRF24L01+ current configuration (for debug purposes)
// in tx_printf the tmp_buff size has to be 512 bytes!!
void NRF24L01::DumpConfig(void) {
	/*	uint8_t i,j;
	 uint8_t aw;
	 uint8_t buf[5];

	 // Dump nRF24L01+ configuration
	 // CONFIG
	 i = ReadReg(nRF24_REG_CONFIG);
	 tx_buff_clear();
	 tx_printf("[0x%02X] 0x%02X  MASK:%03b CRC:%02b ",
	 nRF24_REG_CONFIG,
	 i,
	 i >> 4,
	 (i & 0x0c) >> 2
	 );
	 (i & 0x02) ? tx_printf("PWR: ON ") : tx_printf("PWR: OFF ");
	 (i & 0x01) ? tx_printf("Mode: RX\n") : tx_printf("Mode: TX\n");
	 tx_cycle(); OsHelpers::delay(10);
	 // EN_AA
	 i = ReadReg(nRF24_REG_EN_AA);
	 tx_printf("[0x%02X] 0x%02X ENAA: ",nRF24_REG_EN_AA,i);
	 for (j = 0; j < 6; j++) {
	 tx_printf("[P%1u%s]%s",j,
	 (i & (1 << j)) ? "+" : "-",
	 (j == 5) ? "\n" : " "
	 );
	 }
	 tx_cycle(); OsHelpers::delay(10);
	 // EN_RXADDR
	 i = ReadReg(nRF24_REG_EN_RXADDR);
	 tx_printf("[0x%02X] 0x%02X EN_RXADDR: ",nRF24_REG_EN_RXADDR,i);
	 for (j = 0; j < 6; j++) {
	 tx_printf("[P%1u%s]%s",j,
	 (i & (1 << j)) ? "+" : "-",
	 (j == 5) ? "\n" : " "
	 );
	 }
	 tx_cycle(); OsHelpers::delay(10);
	 // SETUP_AW
	 i = ReadReg(nRF24_REG_SETUP_AW);
	 aw = (i & 0x03) + 2;
	 tx_printf("[0x%02X] 0x%02X EN_RXADDR=%06b (address width = %u)\n",nRF24_REG_SETUP_AW,i,i & 0x03,aw);
	 tx_cycle(); OsHelpers::delay(10);
	 // SETUP_RETR
	 i = ReadReg(nRF24_REG_SETUP_RETR);
	 tx_printf("[0x%02X] 0x%02X ARD=%04b ARC=%04b (retr.delay=%uus, count=%u)\n",
	 nRF24_REG_SETUP_RETR,
	 i,
	 i >> 4,
	 i & 0x0F,
	 ((i >> 4) * 250) + 250,
	 i & 0x0F
	 );
	 tx_cycle(); OsHelpers::delay(10);
	 // RF_CH
	 i = ReadReg(nRF24_REG_RF_CH);
	 tx_printf("[0x%02X] 0x%02X (%.3uGHz)\n",nRF24_REG_RF_CH,i,2400 + i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RF_SETUP
	 i = ReadReg(nRF24_REG_RF_SETUP);
	 tx_printf("[0x%02X] 0x%02X ",
	 nRF24_REG_RF_SETUP,
	 i
	 );
	 tx_cycle(); OsHelpers::delay(10);
	 (i & 0x80) ? tx_printf("CONT_WAVE: ON ") : tx_printf("CONT_WAVE: OFF ");
	 (i & 0x80) ? tx_printf("PLL_LOCK: ON ") : tx_printf("PLL_LOCK: OFF ");
	 tx_cycle(); OsHelpers::delay(10);
	 tx_printf("DataRate=");
	 switch ((i & 0x28) >> 3) {
	 case 0x00:
	 tx_printf("1M");
	 break;
	 case 0x01:
	 tx_printf("2M");
	 break;
	 case 0x04:
	 tx_printf("250k");
	 break;
	 default:
	 tx_printf("???");
	 break;
	 }
	 tx_cycle(); OsHelpers::delay(10);
	 tx_printf("bps RF_PWR=");
	 switch ((i & 0x06) >> 1) {
	 case 0x00:
	 tx_printf("-18");
	 break;
	 case 0x01:
	 tx_printf("-12");
	 break;
	 case 0x02:
	 tx_printf("-6");
	 break;
	 case 0x03:
	 tx_printf("0");
	 break;
	 default:
	 tx_printf("???");
	 break;
	 }
	 tx_printf("dBm\n");
	 tx_cycle(); OsHelpers::delay(10);
	 // STATUS
	 i = ReadReg(nRF24_REG_STATUS);
	 tx_printf("[0x%02X] 0x%02X IRQ:%03b RX_PIPE:%u ",
	 nRF24_REG_STATUS,
	 i,
	 (i & 0x70) >> 4,
	 (i & 0x0E) >> 1
	 );
	 (i & 0x01) ? tx_printf("TX_FULL: YES ") : tx_printf("TX_FULL: NO\n");
	 tx_cycle(); OsHelpers::delay(10);
	 // OBSERVE_TX
	 i = ReadReg(nRF24_REG_OBSERVE_TX);
	 tx_printf("[0x%02X] 0x%02X PLOS_CNT=%u ARC_CNT=%u\n",nRF24_REG_OBSERVE_TX,i,i >> 4,i & 0x0F);
	 tx_cycle(); OsHelpers::delay(10);
	 // RPD
	 i = ReadReg(nRF24_REG_RPD);
	 tx_printf("[0x%02X] 0x%02X RPD=%s\n",nRF24_REG_RPD,i,(i & 0x01) ? "YES" : "NO");
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_ADDR_P0
	 ReadMBReg(nRF24_REG_RX_ADDR_P0,buf,aw);
	 tx_printf("[0x%02X] RX_ADDR_P0 \"",nRF24_REG_RX_ADDR_P0);
	 for (i = 0; i < aw; i++) tx_printf("%c",buf[i]);
	 tx_printf("\"\n");
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_ADDR_P1
	 ReadMBReg(nRF24_REG_RX_ADDR_P1,buf,aw);
	 tx_printf("[0x%02X] RX_ADDR_P1 \"",nRF24_REG_RX_ADDR_P1);
	 for (i = 0; i < aw; i++) tx_printf("%c",buf[i]);
	 tx_printf("\"\n");
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_ADDR_P2
	 tx_printf("[0x%02X] RX_ADDR_P2 \"",nRF24_REG_RX_ADDR_P2);
	 for (i = 0; i < aw - 1; i++) tx_printf("%c",buf[i]);
	 i = ReadReg(nRF24_REG_RX_ADDR_P2);
	 tx_printf("%c\"\n",i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_ADDR_P3
	 tx_printf("[0x%02X] RX_ADDR_P3 \"",nRF24_REG_RX_ADDR_P3);
	 for (i = 0; i < aw - 1; i++) tx_printf("%c",buf[i]);
	 i = ReadReg(nRF24_REG_RX_ADDR_P3);
	 tx_printf("%c\"\n",i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_ADDR_P4
	 tx_printf("[0x%02X] RX_ADDR_P4 \"",nRF24_REG_RX_ADDR_P4);
	 for (i = 0; i < aw - 1; i++) tx_printf("%c",buf[i]);
	 i = ReadReg(nRF24_REG_RX_ADDR_P4);
	 tx_printf("%c\"\n",i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_ADDR_P5
	 tx_printf("[0x%02X] RX_ADDR_P5 \"",nRF24_REG_RX_ADDR_P5);
	 for (i = 0; i < aw - 1; i++) tx_printf("%c",buf[i]);
	 i = ReadReg(nRF24_REG_RX_ADDR_P5);
	 tx_printf("%c\"\n",i);
	 tx_cycle(); OsHelpers::delay(10);
	 // TX_ADDR
	 ReadMBReg(nRF24_REG_TX_ADDR,buf,aw);
	 tx_printf("[0x%02X] TX_ADDR \"",nRF24_REG_TX_ADDR);
	 for (i = 0; i < aw; i++) tx_printf("%c",buf[i]);
	 tx_printf("\"\n");
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_PW_P0
	 i = ReadReg(nRF24_REG_RX_PW_P0);
	 tx_printf("[0x%02X] RX_PW_P0=%u\n",nRF24_REG_RX_PW_P0,i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_PW_P1
	 i = ReadReg(nRF24_REG_RX_PW_P1);
	 tx_printf("[0x%02X] RX_PW_P1=%u\n",nRF24_REG_RX_PW_P1,i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_PW_P2
	 i = ReadReg(nRF24_REG_RX_PW_P2);
	 tx_printf("[0x%02X] RX_PW_P2=%u\n",nRF24_REG_RX_PW_P2,i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_PW_P3
	 i = ReadReg(nRF24_REG_RX_PW_P3);
	 tx_printf("[0x%02X] RX_PW_P3=%u\n",nRF24_REG_RX_PW_P3,i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_PW_P4
	 i = ReadReg(nRF24_REG_RX_PW_P4);
	 tx_printf("[0x%02X] RX_PW_P4=%u\n",nRF24_REG_RX_PW_P4,i);
	 tx_cycle(); OsHelpers::delay(10);
	 // RX_PW_P5
	 i = ReadReg(nRF24_REG_RX_PW_P5);
	 tx_printf("[0x%02X] RX_PW_P5=%u\n",nRF24_REG_RX_PW_P5,i);
	 tx_cycle(); OsHelpers::delay(10); */
}

/*
 >Station is Slave, ID is 256, name: SLAVE_01
 [0x00] 0x0F  MASK:00b CRC:0b PWR: ON Mode: RX
 [0x01] 0x3F ENAA: [P0+] [P1+] [P2+] [P3+] [P4+] [P5+]
 [0x02] 0x03 EN_RXADDR: [P0+] [P1+] [P2-] [P3-] [P4-] [P5-]
 [0x03] 0x03 EN_RXADDR=00000b (address width = 3)
 [0x04] 0x18 ARD=000b ARC=000b (retr.delay=1us, count=8)
 [0x05] 0x76 (2518GHz)
 [0x06] 0x20 CONT_WAVE: OFF PLL_LOCK: OFF DataRate=250k pbs RF_PWR=-18dBm
 [0x07] 0x0E IRQ:00b RX_PIPE:0 TX_FULL: NO
 [0x08] 0x00 PLOS_CNT=0 ARC_CNT=0
 [0x09] 0x00 RPD=NO
 [0x0A] RX_ADDR_P0 "ZXBSE"
 [0x0B] RX_ADDR_P1 "▒▒▒▒▒"
 [0x0C] RX_ADDR_P2 "▒▒▒▒▒"
 [0x0D] RX_ADDR_P3 "▒▒▒▒▒"
 [0x0E] RX_ADDR_P4 "▒▒▒▒▒"
 [0x0F] RX_ADDR_P5 "▒▒▒▒▒"
 [0x10] TX_ADDR "ZXBSE"
 [0x11] RX_PW_P0=0
 [0x12] RX_PW_P1=18
 [0x13] RX_PW_P2=0
 [0x14] RX_PW_P3=0
 [0x15] RX_PW_P4=0
 [0x16] RX_PW_P5=0

 >Station is Master, ID is 1, name: MASTER
 [0x00] 0x0F  MASK:00b CRC:0b PWR: ON Mode: RX
 [0x01] 0x3F ENAA: [P0+] [P1+] [P2+] [P3+] [P4+] [P5+]
 [0x02] 0x3F EN_RXADDR: [P0+] [P1+] [P2+] [P3+] [P4+] [P5+]
 [0x03] 0x03 EN_RXADDR=00000b (address width = 3)
 [0x04] 0x18 ARD=000b ARC=000b (retr.delay=1us, count=8)
 [0x05] 0x76 (2518GHz)
 [0x06] 0x20 CONT_WAVE: OFF PLL_LOCK: OFF DataRate=250k pbs RF_PWR=-18dBm
 [0x07] 0x0E IRQ:00b RX_PIPE:0 TX_FULL: NO
 [0x08] 0x00 PLOS_CNT=0 ARC_CNT=0
 [0x09] 0x00 RPD=NO
 [0x0A] RX_ADDR_P0 "▒▒▒▒▒"
 [0x0B] RX_ADDR_P1 "ZXBSE"
 [0x0C] RX_ADDR_P2 "ZXBSY"
 [0x0D] RX_ADDR_P3 "ZXBSX"
 [0x0E] RX_ADDR_P4 "ZXBSW"
 [0x0F] RX_ADDR_P5 "ZXBSV"
 [0x10] TX_ADDR "nRF24"
 [0x11] RX_PW_P0=18
 [0x12] RX_PW_P1=18
 [0x13] RX_PW_P2=18
 [0x14] RX_PW_P3=18
 [0x15] RX_PW_P4=18
 [0x16] RX_PW_P5=18


 */
#endif
