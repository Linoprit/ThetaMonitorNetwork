/*
 * Eeprom_at24c256_socket.cpp
 *  Created on: 03.03.2021
 *     Author: harald
 */

#include <string.h>
#include <Devices/Eeprom_at24c256/Eeprom_at24c256_socket.h>

/* The at24cxx type memories are organized in pages. If you write in sequence and the
 * write-address exeeds
 */

constexpr uint16_t MAX_ADDRESS = (256 * 1024 / 8) - 1; // at24c256 highest address

Eeprom_at24c256_socket::Eeprom_at24c256_socket ():
_actAddress { 0 },
_writeIsDisabled { true }
{
	_buffer = new uint8_t[MAX_ADDRESS + 1U];
	memset((void*) _buffer, 0xFF, MAX_ADDRESS + 1U);
}



void Eeprom_at24c256_socket::put_address(uint16_t address)
{
	if (address > MAX_ADDRESS)
		address = MAX_ADDRESS;

	_actAddress = address;
}

void Eeprom_at24c256_socket::erase(uint16_t address)
{
	if (_writeIsDisabled)
		return;

	_buffer[_actAddress] = 0xFF;
}

void Eeprom_at24c256_socket::eraseall(void)
{
	if (_writeIsDisabled)
		return;

	memset((void*) _buffer, 0xFF, MAX_ADDRESS + 1U);
}

void Eeprom_at24c256_socket::write_disable(void)
{
	_writeIsDisabled = true;
}

void Eeprom_at24c256_socket::write_enable(void)
{
	_writeIsDisabled = false;
}

uint8_t Eeprom_at24c256_socket::read(uint16_t address)
{
  put_address(address);
  return _buffer[_actAddress];
}

void Eeprom_at24c256_socket::readBlock(uint16_t address, uint8_t *data, uint8_t len)
{
  put_address(address);

  for (uint8_t j=0; j < len; j++) {
	  data[j]  = _buffer[_actAddress];
	 _actAddress++;
  }
}

void Eeprom_at24c256_socket::write(uint16_t address, uint8_t data)
{
  put_address(address);
  if (_writeIsDisabled)
	  return;

  _buffer[_actAddress] = data;
}

void Eeprom_at24c256_socket::writeBlock(uint16_t address, uint8_t* data, uint8_t len) {
	uint16_t tmpAddress = address;
	for (uint8_t i=0; i < len; i++) {
		write(tmpAddress, data[i]);
		tmpAddress++;
	}
}

void Eeprom_at24c256_socket::writeall(uint8_t data)
{
	if (_writeIsDisabled)
		return;

	memset((void*) _buffer, data, MAX_ADDRESS + 1U);
}

uint16_t Eeprom_at24c256_socket::getMaxAddress(void) { return MAX_ADDRESS; }


