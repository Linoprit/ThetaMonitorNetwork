#include <System/Devices/ds18b20/DS18B20.h>
#include <System/serialPrintf.h>
#include <System/OsHelpers.h>

namespace oneWire {

DS18B20::DS18B20() :
		_ow { nullptr }, _foundSensors { 0 } {
}

DS18B20::DS18B20(OneWire *oneWire) :
		_ow { oneWire }, _foundSensors { 0 } {
	for (uint8_t i = 0; i < DS18B20_MAX_DEVICES; i++) {
		_sensors[i].dataIsValid = false;
	}
}

/*
 * Scans for connected _sensors, and writes the found addresses
 * into the _sensors array.
 * Returns amount of found sensors.
 */
uint8_t DS18B20::findAllSensors(void) {
	bool searchFirst = searchFirstWithRetry();
	if (searchFirst == false) { // Timeout
		tx_printf("Searching timed out.\n");
		return 0;
	}

	bool searchSuccess = false;
	for (uint8_t i = 0; i < DS18B20_MAX_DEVICES; i++) {
		_ow->owDelay(100);
		_foundSensors++;
		_ow->getFullROM(_sensors[i].address);

		tx_printf("Found ROM addr: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
				_sensors[i].address[0], _sensors[i].address[1],
				_sensors[i].address[2], _sensors[i].address[3],
				_sensors[i].address[4], _sensors[i].address[5],
				_sensors[i].address[6], _sensors[i].address[7]);

		searchSuccess = _ow->next();

		if (searchSuccess == false) // last device
			break;
	}
	return _foundSensors;
}

bool DS18B20::searchFirstWithRetry(void) {
	bool searchSuccess = false;
	for (uint8_t i = 0; i < DS18B20_SEARCH_RETRIES; i++) {
		_ow->InitLine();
		searchSuccess = _ow->first();
		tx_printf("Searching sensors, retries: %i\n", i);
		if (searchSuccess == 1)
			return true;
	}
	return false;
}

bool DS18B20::setAllResolution(Resolution_t resolution) {
	if (_foundSensors == 0)
		return false;

	for (uint8_t i = 0; i < _foundSensors; i++) {
		osDelay(50);
		setResolution(_sensors[i].address, resolution);
		osDelay(50);
		disableAlarmTemperature(_sensors[i].address);
	}
	return true;
}

uint8_t DS18B20::start(uint8_t *ROM) {
	if (!isDs18b20(ROM)) {
		return 0;
	}
	_ow->reset();
	_ow->selectWithPointer(ROM);
	_ow->writeByte(CMD_CONVERTTEMP);

	return 1;
}

bool DS18B20::doAllMeasure(void) {
	uint16_t timeout = DS18B20_CONVERSION_TIMEOUT_MS / 10;
	startAll();
	osDelay(100);

	while (!allDone()) {
		osDelay(10);
		timeout -= 1;
		if(timeout <= 0){
			break;
		}
	}
	if (timeout == 0) {
		for (uint8_t i = 0; i < _foundSensors; i++) {
			_sensors[i].dataIsValid = false;
		}
		return false;
	}
	for (uint8_t i = 0; i < _foundSensors; i++) {
		osDelay(10);
		_sensors[i].dataIsValid = read(_sensors[i].address,
				&_sensors[i].temperature);
	}
	return true;
}

void DS18B20::startAll(void) {
	_ow->reset();
	_ow->writeByte(OneWire::CMD_SKIPROM);
	// Start conversion on all connected devices
	_ow->writeByte(CMD_CONVERTTEMP);
}

bool DS18B20::read(uint8_t *ROM, float *destination) {
	uint16_t temperature;
	uint8_t resolution;
	int8_t digit, minus = 0;
	float decimal;
	uint8_t i = 0;
	uint8_t data[9];
	uint8_t crc;

	if (!isDs18b20(ROM)) {
		return false;
	}

	// if line is released, conversion is complete
	if (!_ow->readBit()) {
		return false;
	}
	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Read scratchpad command
	_ow->writeByte(OneWire::CMD_RSCRATCHPAD);

	// Get data
	for (i = 0; i < 9; i++) {
		data[i] = _ow->readByte();
	}
	crc = _ow->CRC8(data, 8);

	if (crc != data[8])
		return 0; // CRC invalid

	// First two bytes of scratchpad are temperature values
	temperature = data[0] | (data[1] << 8);

	_ow->reset();

	// Check if temperature is negative
	if (temperature & 0x8000) {
		// Two's complement, temperature is negative
		temperature = ~temperature + 1;
		minus = 1;
	}

	// Get sensor resolution
	resolution = ((data[4] & 0x60) >> 5) + 9;

	// Store temperature integer digits and decimal digits
	digit = temperature >> 4;
	digit |= ((temperature >> 8) & 0x7) << 4;

	// Store decimal digits
	switch (resolution) {
	case 9:
		decimal = (temperature >> 3) & 0x01;
		decimal *= DECIMAL_STEPS_9BIT;
		break;
	case 10:
		decimal = (temperature >> 2) & 0x03;
		decimal *= DECIMAL_STEPS_10BIT;
		break;
	case 11:
		decimal = (temperature >> 1) & 0x07;
		decimal *= DECIMAL_STEPS_11BIT;
		break;
	case 12:
		decimal = temperature & 0x0F;
		decimal *= DECIMAL_STEPS_12BIT;
		break;
	default:
		decimal = 0xFF;
		digit = 0;
	}

	// Check for negative part
	decimal = digit + decimal;
	if (minus) {
		decimal = 0 - decimal;
	}

	*destination = decimal;

	return true; // Return 1, temperature valid
}

uint8_t DS18B20::getResolution(uint8_t *ROM) {
	uint8_t conf;

	if (!isDs18b20(ROM)) {
		return 0;
	}
	_ow->reset();
	_ow->selectWithPointer(ROM);
	_ow->writeByte(OneWire::CMD_RSCRATCHPAD);

	// Ignore first 4 bytes
	_ow->readByte();
	_ow->readByte();
	_ow->readByte();
	_ow->readByte();

	// 5th byte of scratchpad is configuration register
	conf = _ow->readByte();

	// Return 9 - 12 value according to number of bits
	return ((conf & 0x60) >> 5) + 9;
}

uint8_t DS18B20::setResolution(uint8_t *ROM, Resolution_t resolution) {
	uint8_t th, tl, conf;
	if (!isDs18b20(ROM)) {
		return 0;
	}
	_ow->reset();
	_ow->selectWithPointer(ROM);
	_ow->writeByte(OneWire::CMD_RSCRATCHPAD);

	// Ignore first 2 bytes
	_ow->readByte();
	_ow->readByte();

	th = _ow->readByte();
	tl = _ow->readByte();
	conf = _ow->readByte();

	if (resolution == Resolution_9bits) {
		conf &= ~(1 << RESOLUTION_R1);
		conf &= ~(1 << RESOLUTION_R0);
	} else if (resolution == Resolution_10bits) {
		conf &= ~(1 << RESOLUTION_R1);
		conf |= 1 << RESOLUTION_R0;
	} else if (resolution == Resolution_11bits) {
		conf |= 1 << RESOLUTION_R1;
		conf &= ~(1 << RESOLUTION_R0);
	} else if (resolution == Resolution_12bits) {
		conf |= 1 << RESOLUTION_R1;
		conf |= 1 << RESOLUTION_R0;
	}

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Write scratchpad command, only th, tl and conf register can be written
	_ow->writeByte(OneWire::CMD_WSCRATCHPAD);

	_ow->writeByte(th);
	_ow->writeByte(tl);
	_ow->writeByte(conf);

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Copy scratchpad to EEPROM of DS18B20
	_ow->writeByte(OneWire::CMD_CPYSCRATCHPAD);

	return 1;
}

uint8_t DS18B20::isDs18b20(uint8_t *ROM) {
	// Checks if first byte is equal to DS18B20's family code
	if (*ROM == FAMILY_CODE)
		return 1;

	return 0;
}

uint8_t DS18B20::setAlarmLowTemperature(uint8_t *ROM, int8_t temp) {
	uint8_t tl, th, conf;
	if (!isDs18b20(ROM))
		return 0;

	if (temp > 125)
		temp = 125;

	if (temp < -55)
		temp = -55;

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Read scratchpad command
	_ow->writeByte(OneWire::CMD_RSCRATCHPAD);

	// Ignore first 2 bytes
	_ow->readByte();
	_ow->readByte();

	th = _ow->readByte();
	tl = _ow->readByte();
	conf = _ow->readByte();

	tl = (uint8_t) temp;

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Write scratchpad command, only th, tl and conf register can be written
	_ow->writeByte(OneWire::CMD_WSCRATCHPAD);

	_ow->writeByte(th);
	_ow->writeByte(tl);
	_ow->writeByte(conf);

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Copy scratchpad to EEPROM of DS18B20
	_ow->writeByte(OneWire::CMD_CPYSCRATCHPAD);

	return 1;
}

uint8_t DS18B20::setAlarmHighTemperature(uint8_t *ROM, int8_t temp) {
	uint8_t tl, th, conf;
	if (!isDs18b20(ROM))
		return 0;

	if (temp > 125)
		temp = 125;

	if (temp < -55)
		temp = -55;

	_ow->reset();
	_ow->selectWithPointer(ROM);
	_ow->writeByte(OneWire::CMD_RSCRATCHPAD);

	// Ignore first 2 bytes
	_ow->readByte();
	_ow->readByte();

	th = _ow->readByte();
	tl = _ow->readByte();
	conf = _ow->readByte();

	th = (uint8_t) temp;

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Write scratchpad command, only th, tl and conf register can be written
	_ow->writeByte(OneWire::CMD_WSCRATCHPAD);

	_ow->writeByte(th);
	_ow->writeByte(tl);
	_ow->writeByte(conf);

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Copy scratchpad to EEPROM of DS18B20
	_ow->writeByte(OneWire::CMD_CPYSCRATCHPAD);

	return 1;
}

uint8_t DS18B20::disableAlarmTemperature(uint8_t *ROM) {
	uint8_t tl, th, conf;
	if (!isDs18b20(ROM))
		return 0;

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Read scratchpad command
	_ow->writeByte(OneWire::CMD_RSCRATCHPAD);

	// Ignore first 2 bytes
	_ow->readByte();
	_ow->readByte();

	th = _ow->readByte();
	tl = _ow->readByte();
	conf = _ow->readByte();

	th = 125;
	tl = (uint8_t) -55;

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Write scratchpad command, only th, tl and conf register can be written
	_ow->writeByte(OneWire::CMD_WSCRATCHPAD);

	_ow->writeByte(th);
	_ow->writeByte(tl);
	_ow->writeByte(conf);

	_ow->reset();
	_ow->selectWithPointer(ROM);
	// Copy scratchpad to EEPROM of DS18B20
	_ow->writeByte(OneWire::CMD_CPYSCRATCHPAD);

	return 1;
}

uint8_t DS18B20::alarmSearch() {
	// Start alarm search
	return _ow->search(CMD_ALARMSEARCH);
}

uint8_t DS18B20::allDone() {
	// If read bit is low, then device is not finished yet with calculation temperature
	return _ow->readBit();
}

} //namespace oneWire

