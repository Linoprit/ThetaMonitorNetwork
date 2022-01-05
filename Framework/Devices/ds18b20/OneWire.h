/*
 * OneWire.h
 *  Created on: Nov 1, 2021
 *      Author: harald
 * original code is from Ko-fi: https://github.com/nimaltd/ds18b20
 */
#ifndef ONEWIRE_H_
#define ONEWIRE_H_


#ifdef __x86_64
#include "stm32f1xx_hal.h"
#elif defined STM32F401xE
#include "stm32f4xx.h"
#elif defined STM32F103xB
#include <stm32f1xx.h>
#endif
#include <Config/config.h>

namespace oneWire {

class OneWire {
public:
	OneWire();
	OneWire(GPIO_TypeDef *GPIO_TX_PORT, uint16_t GPIO_TX_Pin,
			GPIO_TypeDef *GPIO_RX_PORT, uint16_t GPIO_RX_Pin);
	virtual ~OneWire() {
	}
	;

	/* OneWire commands */
	static constexpr uint8_t CMD_RSCRATCHPAD = 0xBE;
	static constexpr uint8_t CMD_WSCRATCHPAD = 0x4E;
	static constexpr uint8_t CMD_CPYSCRATCHPAD = 0x48;
	static constexpr uint8_t CMD_RECEEPROM = 0xB8;
	static constexpr uint8_t CMD_RPWRSUPPLY = 0xB4;
	static constexpr uint8_t CMD_SEARCHROM = 0xF0;
	static constexpr uint8_t CMD_READROM = 0x33;
	static constexpr uint8_t CMD_MATCHROM = 0x55;
	static constexpr uint8_t CMD_SKIPROM = 0xCC;

	inline void owDelay(uint16_t time_us) {
		OW_TIMER->Instance->CNT = 0;
		while (OW_TIMER->Instance->CNT <= time_us)
			;
	}

	// pull line
	inline void lineLow(void) {
#if (OW_LINE_INVERTED == true)
		HAL_GPIO_WritePin(_GPIO_TX_PORT, _GPIO_TX_Pin, GPIO_PIN_RESET);
	#else
		HAL_GPIO_WritePin(_GPIO_TX_PORT, _GPIO_TX_Pin, GPIO_PIN_SET);
#endif
	}

	// release line
	inline void lineHigh(void) {
#if (OW_LINE_INVERTED == true)
		HAL_GPIO_WritePin(_GPIO_TX_PORT, _GPIO_TX_Pin, GPIO_PIN_SET);
	#else
		HAL_GPIO_WritePin(_GPIO_TX_PORT, _GPIO_TX_Pin, GPIO_PIN_RESET);
#endif
	}
	inline bool readLine(void) {
#if (OW_LINE_INVERTED == true)
		return HAL_GPIO_ReadPin(_GPIO_RX_PORT, _GPIO_RX_Pin);
	#else
		return !HAL_GPIO_ReadPin(_GPIO_RX_PORT, _GPIO_RX_Pin);
#endif
	}

	void initTimer(void);
	void InitLine(void);
	uint8_t reset(void);
	void writeBit(uint8_t bit);
	bool readBit(void);
	void writeByte(uint8_t byte);
	uint8_t readByte(void);
	bool first(void);
	bool next(void);
	void resetSearch(void);
	bool search(uint8_t command);
	int verify(void);
	void targetSetup(uint8_t family_code);
	void familySkipSetup();
	uint8_t getROM(uint8_t index);
	void select(uint8_t *addr);
	void selectWithPointer(uint8_t *ROM);
	void getFullROM(uint8_t *firstIndex);
	uint8_t CRC8(uint8_t *addr, uint8_t len);

private:
	GPIO_TypeDef *_GPIO_TX_PORT;
	uint16_t _GPIO_TX_Pin;
	GPIO_TypeDef *_GPIO_RX_PORT;
	uint16_t _GPIO_RX_Pin;

	uint8_t _lastDiscrepancy; /*!< Search private */
	uint8_t _lastFamilyDiscrepancy; /*!< Search private */
	uint8_t _lastDeviceFlag; /*!< Search private */
	uint8_t ROM_NO[8]; /*!< 8-bytes address of last search device */

};

} // namespacee oneWire
#endif /* ONEWIRE_H_ */
