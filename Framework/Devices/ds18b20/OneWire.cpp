/*
 * OneWire.cpp
 *
 *  Created on: Nov 1, 2021
 *      Author: harald
 */

#include "OneWire.h"
#include <System/OsHelpers.h>
#include "cmsis_os.h" // for osDelay

namespace oneWire {

// @formatter:off
OneWire::OneWire() :
		_GPIO_TX_PORT { nullptr },
		_GPIO_TX_Pin { 0 },
		_GPIO_RX_PORT { nullptr },
		_GPIO_RX_Pin { 0 }
{ }
OneWire::OneWire(
		GPIO_TypeDef *GPIO_TX_PORT,  uint16_t GPIO_TX_Pin,
		GPIO_TypeDef *GPIO_RX_PORT,  uint16_t GPIO_RX_Pin) :
		_GPIO_TX_PORT { GPIO_TX_PORT },
		_GPIO_TX_Pin { GPIO_TX_Pin },
		_GPIO_RX_PORT { GPIO_RX_PORT },
		_GPIO_RX_Pin { GPIO_RX_Pin }
{ }
// @formatter:on

void OneWire::initTimer(void) {
	HAL_TIM_Base_Start (OW_TIMER);
}

void OneWire::InitLine(void) {
	lineHigh();
	OsHelpers::delay(1000);
	lineLow();
	OsHelpers::delay(1000);
	lineHigh();
	OsHelpers::delay(2000);
}

uint8_t OneWire::reset(void) {
	uint8_t i;

	lineLow();
	owDelay(480);

	lineHigh();
	owDelay(70);

	i = (uint8_t)  readLine();
	owDelay(410); // second delay

	// result of presence pulse, 0 = device detected, 1 = no device present
	return i;
}

void OneWire::writeBit(uint8_t bit) {
	if (bit) {
		lineLow();
		owDelay(5);

		lineHigh();
		owDelay(65); // Wait to complete 70us period
	} else {
		lineLow();
		owDelay(65);

		lineHigh();
		owDelay(10); // Wait to complete period (ca. 70us)
	}
}

bool OneWire::readBit(void) {
	bool bit = false;

	lineLow();
	owDelay(4); // critical! 3 - 4

	lineHigh();
	owDelay(5); // was 5

	bit = readLine();
	owDelay(60); // Wait to complete period (ca. 60us)

	return bit;
}

void OneWire::writeByte(uint8_t byte) {
	uint8_t i = 8;

	while (i--) { // Write 8 bits, LSB first
		writeBit(byte & 0x01);
		byte >>= 1;
	}
}

uint8_t OneWire::readByte(void) {
	uint8_t i = 8, byte = 0;
	while (i--) {
		byte >>= 1;
		byte |= ((uint8_t) readBit() << 7);
	}
	return byte;
}

bool OneWire::first(void) {
	resetSearch();
	return search(CMD_SEARCHROM);
}

bool OneWire::next(void) {
   // Leave the search state alone
   return search(CMD_SEARCHROM);
}

void OneWire::resetSearch(void) {
	// Reset the search state
	_lastDiscrepancy = 0;
	_lastDeviceFlag = 0;
	_lastFamilyDiscrepancy = 0;
}

bool OneWire::search(uint8_t command) {
	uint8_t id_bit_number = 1;
	uint8_t last_zero = 0;
	uint8_t rom_byte_number = 0;
	bool searchIsSuccess = 0;
	uint8_t id_bit = 0;
	uint8_t cmp_id_bit = 0;
	uint8_t rom_byte_mask = 1;
	uint8_t search_direction = 0;

	// if the last call was not the final one
	if (!_lastDeviceFlag)
	{
		if (reset() == true) {
			// Reset the search
			_lastDiscrepancy = 0;
			_lastDeviceFlag = 0;
			_lastFamilyDiscrepancy = 0;
			return 0;
		}

		writeByte(command);  // issue the search command

		do { // loop to do the search
			// read one bit and its complement
			id_bit = readBit();
			cmp_id_bit = readBit();

			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1)) {
				break;
			} else {
				// all devices coupled have 0 or 1
				if (id_bit != cmp_id_bit) {
					search_direction = id_bit;  // bit write value for search
				} else {
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (id_bit_number < _lastDiscrepancy) {
						search_direction =
								((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					} else {
						// if equal to last pick 1, if not then pick 0
						search_direction = (id_bit_number == _lastDiscrepancy);
					}

					// if 0 was picked then record its position in LastZero
					if (search_direction == 0) {
						last_zero = id_bit_number;
						// check for Last discrepancy in family
						if (last_zero < 9) {
							_lastFamilyDiscrepancy = last_zero;
						}
					}
				}

				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (search_direction == 1) {
					ROM_NO[rom_byte_number] |= rom_byte_mask;
				} else {
					ROM_NO[rom_byte_number] &= ~rom_byte_mask;
				}

				// serial number search direction write bit
				writeBit(search_direction);

				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				rom_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (rom_byte_mask == 0) {
					//docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		} while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

		// search was successful
		if (!(id_bit_number < 65)) {
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			_lastDiscrepancy = last_zero;

			// check for last device
			if (_lastDiscrepancy == 0) {
				_lastDeviceFlag = 1;
			}
			searchIsSuccess = true;
		}
	}
	// if no device found then reset counters so next 'search' will be like a first
	if (!searchIsSuccess || !ROM_NO[0]) {
		_lastDiscrepancy = 0;
		_lastDeviceFlag = 0;
		_lastFamilyDiscrepancy = 0;
		searchIsSuccess = false;
	}
	return searchIsSuccess;
}

int OneWire::verify(void) {
	unsigned char rom_backup[8];
	int i = 0;
	int rslt = 0;
	int ld_backup = 0;
	int ldf_backup = 0;
	int lfd_backup = 0;

	// keep a backup copy of the current state
	for (i = 0; i < 8; i++) {
		rom_backup[i] = ROM_NO[i];
	}
	ld_backup =  _lastDiscrepancy;
	ldf_backup = _lastDeviceFlag;
	lfd_backup = _lastFamilyDiscrepancy;

	// set search to find the same device
	_lastDiscrepancy = 64;
	_lastDeviceFlag = 0;

	if (search(CMD_SEARCHROM)) {
		// check if same device was found
		rslt = 1;
		for (i = 0; i < 8; i++) {
			if (rom_backup[i] != ROM_NO[i]) {
				rslt = 1;
				break;
			}
		}
	} else {
		rslt = 0;
	}

	for (i = 0; i < 8; i++) { // restore the search state
		ROM_NO[i] = rom_backup[i];
	}
	_lastDiscrepancy = ld_backup;
	_lastDeviceFlag = ldf_backup;
	_lastFamilyDiscrepancy = lfd_backup;

	return rslt; // return result of the verification
}

void OneWire::targetSetup(uint8_t family_code) {
   uint8_t i;

	// set the search state to find SearchFamily type devices
	ROM_NO[0] = family_code;
	for (i = 1; i < 8; i++) {
		ROM_NO[i] = 0;
	}

	_lastDiscrepancy = 64;
	_lastFamilyDiscrepancy = 0;
	_lastDeviceFlag = 0;
}

void OneWire::familySkipSetup() {
	// set the Last discrepancy to last family discrepancy
	_lastDiscrepancy = _lastFamilyDiscrepancy;
	_lastFamilyDiscrepancy = 0;

	// check for end of list
	if (_lastDiscrepancy == 0) {
		_lastDeviceFlag = 1;
	}
}

uint8_t OneWire::getROM(uint8_t index) {
	return ROM_NO[index];
}

void OneWire::select(uint8_t* addr) {
	uint8_t i;
	writeByte(CMD_MATCHROM);

	for (i = 0; i < 8; i++) {
		writeByte(*(addr + i));
	}
}

void OneWire::selectWithPointer(uint8_t *ROM) {
	uint8_t i;
	writeByte(CMD_MATCHROM);

	for (i = 0; i < 8; i++) {
		writeByte(*(ROM + i));
	}
}

void OneWire::getFullROM(uint8_t *firstIndex) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		*(firstIndex + i) = ROM_NO[i];
	}
}

uint8_t OneWire::CRC8(uint8_t *addr, uint8_t len) {
	uint8_t crc = 0;
	uint8_t inbyte = 0;
	uint8_t i = 0;
	uint8_t mix = 0;

	while (len--) {
		inbyte = *addr++;
		for (i = 8; i; i--) {
			mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) {
				crc ^= 0x8C;
			}
			inbyte >>= 1;
		}
	}

	return crc; // Return calculated CRC
}

} // namespace oneWire

