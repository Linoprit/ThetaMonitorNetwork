/*
 * EepromEmulation.cpp
 *
 *  Created on: 03.03.2021
 *      Author: harald
 */

#include "EepromEmulation.h"
#include <System/Error_Handler.h>
#include <string.h>
#include <stdio.h>

uint8_t EepromEmulation::_buffer[MAX_ADDRESS + 1U];
bool EepromEmulation::_doPageWrite = true;

void EepromEmulation::init(void) {
	memset((void*) &_buffer, 0xFF, MAX_ADDRESS + 1U);
	_doPageWrite = true;
}
uint16_t EepromEmulation::getMaxAddress(void) {
	return MAX_ADDRESS;
}

void EepromEmulation::erase(void) {
	memset((void*) &_buffer, 0xFF, MAX_ADDRESS + 1U);
}
void EepromEmulation::setPageWrite(bool onOrOff) {
	_doPageWrite = onOrOff;
}

uint8_t* EepromEmulation::get_buffer(void) {
	return &_buffer[0];
}

uint16_t EepromEmulation::pageAtAddress(uint16_t address) {
	return (address / PAGESIZE);
}

uint16_t EepromEmulation::lowerPageAddress(uint16_t address) {
	return (pageAtAddress(address) * PAGESIZE);
}

uint16_t EepromEmulation::upperPageAddress(uint16_t address) {
	return (lowerPageAddress(address) + PAGESIZE - 1);
}

uint16_t EepromEmulation::lowerAddressFromPageNr(uint16_t pageNr) {
	return (pageNr * PAGESIZE);
}

uint16_t EepromEmulation::upperAddressFromPageNr(uint16_t pageNr) {
	return (lowerAddressFromPageNr(pageNr) + PAGESIZE - 1);
}

void EepromEmulation::write(uint16_t address, uint8_t data) {
	if (address > MAX_ADDRESS) {
		error_handler(__FILE__, __LINE__);
	}
	_buffer[address] = data;
}

void EepromEmulation::write(uint16_t address, uint8_t *data, uint16_t len) {
	if ((address + len - 1) > MAX_ADDRESS) {
		error_handler(__FILE__, __LINE__);
	}
	if (_doPageWrite == false) {
		for (uint16_t i = 0; i < len; i++) {
			_buffer[address + i] = data[i];
		}
	} else {
		// the at24cxx increments the last bits only, depending on the pagesize.
		const uint16_t hiAddrMask = address & ~(PAGESIZE - 1);
		uint16_t addrCount = address & (PAGESIZE - 1);

		for (uint16_t i = 0; i < len; i++) {
			_buffer[hiAddrMask + addrCount] = data[i];
			addrCount++;
			addrCount = addrCount & (PAGESIZE - 1);
		}
	}
}

void EepromEmulation::fillEepromWith(uint8_t value) {
	uint8_t *eepromBuffer = EepromEmulation::get_buffer();
	memset((void*) eepromBuffer, value, EepromEmulation::MAX_ADDRESS + 1U);
}

uint8_t EepromEmulation::read(uint16_t address) {
	if (address > MAX_ADDRESS) {
		error_handler(__FILE__, __LINE__);
	}
	return _buffer[address];
}

void EepromEmulation::read(uint16_t address, uint8_t *data, uint16_t len) {
	if ((address + len - 1) > MAX_ADDRESS) {
		error_handler(__FILE__, __LINE__);
	}
	for (uint16_t i = 0; i < len; i++) {
		data[i] = _buffer[address + i];
	}
}

void EepromEmulation::dump(uint16_t startAddress, uint16_t endAdress) {
	char buf[MAX_ADDRESS + 1];
	memset((void*) &buf, '\0', 127);

	for (uint16_t i = startAddress; i < endAdress + 1; i += 16) {
		if (((i % 64) == 0) && (i > 0))
			printf("\n");

//printf("%02x %02x %02x %02x  %02x %02x %02x %02x   %02x %02x %02x %02x  %02x %02x %02x %02x :%i \n",
		printf(
				"%03i %03i %03i %03i  %03i %03i %03i %03i   %03i %03i %03i %03i  %03i %03i %03i %03i :%i \n",
				_buffer[i], _buffer[i + 1], _buffer[i + 2], _buffer[i + 3],
				_buffer[i + 4], _buffer[i + 5], _buffer[i + 6], _buffer[i + 7],
				_buffer[i + 8], _buffer[i + 9], _buffer[i + 10],
				_buffer[i + 11], _buffer[i + 12], _buffer[i + 13],
				_buffer[i + 14], _buffer[i + 15], i + 15);
	}
}

// C interfaces

void eepromEmulation_init(void){
	EepromEmulation::init();
}
void eepromEmulation_erase(void){
	EepromEmulation::erase();
}
void eepromEmulation_writeByte(uint16_t address, uint8_t data){
	EepromEmulation::write(address, data);
}
void eepromEmulation_write(uint16_t address, uint8_t* data, uint16_t len){
	EepromEmulation::write(address, data, len);
}
uint8_t eepromEmulation_readByte(uint16_t address){
	return EepromEmulation::read(address);
}
void eepromEmulation_read(uint16_t address, uint8_t* data, uint16_t len){
	EepromEmulation::read(address, data, len);
}
uint16_t eepromEmulation_getMaxAddress(void){
	return EepromEmulation::getMaxAddress();
}
uint8_t* eepromEmulation_get_buffer(void){
	return EepromEmulation::get_buffer();
}
uint16_t eepromEmulation_upperPageAddress(uint16_t address){
	return EepromEmulation::upperPageAddress(address);
}
uint16_t eepromEmulation_lowerPageAddress(uint16_t address){
	return EepromEmulation::lowerPageAddress(address);
}
uint16_t eepromEmulation_pageAtAddress(uint16_t address){
	return EepromEmulation::pageAtAddress(address);
}
uint16_t eepromEmulation_lowerAddressFromPageNr(uint16_t pageNr){
	return EepromEmulation::lowerAddressFromPageNr(pageNr);
}
uint16_t eepromEmulation_upperAddressFromPageNr(uint16_t pageNr){
	return EepromEmulation::upperAddressFromPageNr(pageNr);
}
void eepromEmulation_setPageWrite(bool onOrOff){
	EepromEmulation::setPageWrite(onOrOff);
}
void eepromEmulation_fillEepromWith(uint8_t value){
	EepromEmulation::fillEepromWith(value);
}
