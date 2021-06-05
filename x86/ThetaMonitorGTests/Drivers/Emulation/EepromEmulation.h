/*
 * EepromEmulation.h
 *
 *  Created on: 03.03.2021
 *      Author: harald
 */

#ifndef DRIVERS_EMULATION_EEPROMEMULATION_H_
#define DRIVERS_EMULATION_EEPROMEMULATION_H_

#include <stdint.h>

/* The at24cxx type memories are organized in pages. If you write in sequence and the
 * write-address exeeds
 */

// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"

class EepromEmulation {
public:

constexpr static uint16_t MAX_ADDRESS = (256 * 1024 / 8) - 1; // at24c256 highest address
constexpr static uint16_t PAGESIZE = 64; // in Case of at24c256

	virtual ~EepromEmulation() {
	}
	;

	static void init(void);
	static void erase(void);
	static void write(uint16_t address, uint8_t data);
	static void write(uint16_t address, uint8_t* data, uint16_t len);
	static uint8_t read(uint16_t address);
	static void read(uint16_t address, uint8_t* data, uint16_t len);
	static uint16_t getMaxAddress(void);
	static uint8_t* get_buffer(void);
	static void dump(uint16_t startAddress = 0, uint16_t endAdress = MAX_ADDRESS);
	static uint16_t upperPageAddress(uint16_t address);
	static uint16_t lowerPageAddress(uint16_t address);
	static uint16_t pageAtAddress(uint16_t address);
	static uint16_t lowerAddressFromPageNr(uint16_t pageNr);
	static uint16_t upperAddressFromPageNr(uint16_t pageNr);
	static void setPageWrite(bool onOrOff);
	static void fillEepromWith(uint8_t value);

private:
	EepromEmulation() {
	}
	;
	static uint8_t _buffer[MAX_ADDRESS + 1U];
	static bool _doPageWrite;
};


#else
#define EXTERNC
#endif

#include <stdbool.h>
EXTERNC void eepromEmulation_init(void);
EXTERNC void eepromEmulation_erase(void);
EXTERNC void eepromEmulation_writeByte(uint16_t address, uint8_t data);
EXTERNC void eepromEmulation_write(uint16_t address, uint8_t* data, uint16_t len);
EXTERNC uint8_t eepromEmulation_readByte(uint16_t address);
EXTERNC void eepromEmulation_read(uint16_t address, uint8_t* data, uint16_t len);
EXTERNC uint16_t eepromEmulation_getMaxAddress(void);
EXTERNC uint8_t* eepromEmulation_get_buffer(void);
EXTERNC uint16_t eepromEmulation_upperPageAddress(uint16_t address);
EXTERNC uint16_t eepromEmulation_lowerPageAddress(uint16_t address);
EXTERNC uint16_t eepromEmulation_pageAtAddress(uint16_t address);
EXTERNC uint16_t eepromEmulation_lowerAddressFromPageNr(uint16_t pageNr);
EXTERNC uint16_t eepromEmulation_upperAddressFromPageNr(uint16_t pageNr);
EXTERNC void eepromEmulation_setPageWrite(bool onOrOff);

#endif /* DRIVERS_EMULATION_EEPROMEMULATION_H_ */
