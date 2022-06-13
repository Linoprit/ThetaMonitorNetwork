/*
 * Harald Graef, 06.03.2021
 * Lib for writing to AT24Cxx Eeprom Devices.
 * Handles the roll-over, when doing sequential writing beyond the page-limits.
 */

#ifndef AT24C256_H_
#define AT24C256_H_

#ifdef __x86_64
#include "stm32f1xx_hal.h"
#elif defined STM32F401xE
#include "stm32f4xx.h"
#elif defined STM32F103xB
#include "stm32f1xx.h"
#endif
#include <Config/config.h>

class AT24Cxxx {
public:
	// user configuration
	constexpr static I2C_HandleTypeDef *I2C_HANDLE = &EEPROM_HI2C;
	constexpr static uint8_t DEVICE_ADDRESS = EEPROM_I2C_ADDRESS;

	// for the following, refer to the datasheet
	// at24 device highest memory-address
	// i.e. AT24C256 has 256kBit, so the address range is
	// from 0 to (256 * 1024 / 8) - 1 = 32767
	constexpr static uint16_t MAX_ADDRESS = (256 * 1024 / 8) - 1;
	constexpr static uint16_t E2PAGESIZE = 64; // = 64 Bytes, in Case of at24c256

// @formatter:off
	AT24Cxxx() {};
	virtual ~AT24Cxxx() {};
// @formatter:on

	static void write(uint16_t startAddr, uint8_t *data, uint32_t len);
	static void read(uint16_t startAddr, uint8_t *data, uint32_t len);
	static void erase(uint16_t startAddr, uint32_t len);

	// Page related functions, you normally don't need them.
	// Given an address, it returns in which page it resides.
	static uint16_t pageAtAddress(uint16_t address);
	// Given an address, it returns the lowest address of the page, the address is in.
	static uint16_t lowerPageAddress(uint16_t address);
	// Given an address, it returns the highest address of the page, the address is in.
	static uint16_t upperPageAddress(uint16_t address);
	// Given a page, it returns the lowest address of the page, the address is in.
	static uint16_t lowerAddressFromPageNr(uint16_t pageNr);
	// Given a page, it returns the highest address of the page, the address is in.
	static uint16_t upperAddressFromPageNr(uint16_t pageNr);

};

#endif /* AT24C256_H_ */
