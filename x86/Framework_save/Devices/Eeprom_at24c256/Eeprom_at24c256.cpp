/*
 * Eeprom_at24c256.cpp
 *  Created on: 03.03.2021
 *      Author: harald
 */

#include <Devices/Eeprom_at24c256/Eeprom_at24c256.h>
#include <Sockets/CrcSocket.h>

namespace eeprom {
Eeprom_at24c256::Eeprom_at24c256(void) {
	_socket = NULL;
	_nextDataAddress = 0;
	_numOfJournalEntries = 0;
}

Eeprom_at24c256::Eeprom_at24c256(Eeprom_at24c256_socket *socket) {
	_socket = socket;
	_nextDataAddress = sizeof(JournalInfoType)
			+ sizeof(JournalEntryType) * JOURNAL_SIZE;
	_numOfJournalEntries = 0;
}

/** returns false, if wrong journal-format is read */
bool Eeprom_at24c256::init(void) {
	if (!journalInfoIsValid()) {
		return false;
	}

	_socket->write_enable(); // never forget!

	// read all journals into the Map-class
	JournalEntryType journalEntry;
	for (uint8_t i = 0; i < JOURNAL_SIZE; i++) {
		if (readJournalEntry(i, journalEntry) == true) {
			_numOfJournalEntries = i;
			_nextDataAddress = journalEntry.startAddress + journalEntry.size;
			_journalEntryMap.put(journalEntry);
		} else {
			break;
		}
	}
	return true;
}

/** returns true, if data for id was found and written to *buffer */
bool Eeprom_at24c256::readData(ValueID id, uint8_t *buffer) {
	JournalEntryType journalEntry = _journalEntryMap.get(id);

	CrcType calcCrc = CrcSocket::calc_chksum(
			reinterpret_cast<uint8_t*>(&journalEntry),
			sizeof(JournalEntryType) - sizeof(CrcType));

	if ((journalEntry.isInvalid()) || (calcCrc != journalEntry.crc)) {
		return false;
	}

	_socket->readBlock(journalEntry.startAddress, buffer, journalEntry.size);
	if (!blockIsValid(buffer, journalEntry.size)) {
		return false;
	}
	return true;
}

/** Returns true, if data  was written to EEPROM. If there is no Data-Space,
 *  or no Journal-Space left, false is returned. */
bool Eeprom_at24c256::writeData(ValueID id, uint8_t *buffer, uint8_t size) {
	if (!isDataWritable(size)) {
		return false;
	}
	JournalEntryType journalEntry;
	journalEntry.entryID = static_cast<uint8_t>(id);
	journalEntry.size = size;
	journalEntry.startAddress = getNextDataAddress();
	// crc is calculated in write-function

	bool result = writeJournalEntry(journalEntry);
	if (result == false) {
		return false;
	}

	CrcType crc = CrcSocket::calc_chksum(buffer, size - sizeof(CrcType));
	buffer[size - sizeof(CrcType)] = crc;
	_socket->writeBlock(getNextDataAddress(), buffer, size);
	_nextDataAddress = _nextDataAddress + size;

	return true;
}

bool Eeprom_at24c256::writeJournalEntry(JournalEntryType journalEntry) {
	// check again if writable
	if (_numOfJournalEntries >= JOURNAL_SIZE) {
		return false;
	}
	journalEntry.crc = CrcSocket::calc_chksum(
			reinterpret_cast<uint8_t*>(&journalEntry),
			sizeof(JournalEntryType) - sizeof(CrcType));

	_socket->writeBlock(calcNextJournalAddress(),
			reinterpret_cast<uint8_t*>(&journalEntry),
			sizeof(JournalEntryType));

	_journalEntryMap.put(journalEntry);

	_numOfJournalEntries++;
	return true;
}

bool Eeprom_at24c256::readJournalEntry(uint8_t entryNumber,
		JournalEntryType &journalEntry) {

	uint16_t address = sizeof(JournalInfoType)
			+ (entryNumber * sizeof(JournalEntryType));
	_socket->readBlock(address, reinterpret_cast<uint8_t*>(&journalEntry),
			sizeof(JournalEntryType));

	if (!blockIsValid(reinterpret_cast<uint8_t*>(&journalEntry),
			journalEntry.size)) {
		return false;
	}
	return true;
}

/**
 * We overwrite the CRC with 255 and we overwrite the size with 0;
 */
void Eeprom_at24c256::eraseEeprom(void) {

	_socket->write_enable(); // never forget!

	// maybe we have to use the _socket-function 'eraseall'
	// invalidate all journal entries
	uint16_t startAddress = JOURNAL_INFO_ADDRESS + sizeof(JournalInfoType)
			+ sizeof(JournalEntryType) - sizeof(CrcType); // points to the first crc

	for (uint8_t i = 0; i < JOURNAL_SIZE; i++) {
		uint16_t crcAddress = i * sizeof(JournalEntryType) + startAddress;
		_socket->write(crcAddress - sizeof(CrcType), 0);
		_socket->write(crcAddress, 255);
	}
	writeJournalInfo();

	_nextDataAddress = sizeof(JournalInfoType)
			+ sizeof(JournalEntryType) * JOURNAL_SIZE;
	_numOfJournalEntries = 0;

	JournalEntryMap _journalEntryMap;

}

uint16_t Eeprom_at24c256::calcNextJournalAddress(void) {
	if (getFreeJournalEntries() == 0)
		return 0;

	return sizeof(JournalInfoType)
			+ (_numOfJournalEntries * sizeof(JournalEntryType));
}

bool Eeprom_at24c256::isDataWritable(uint16_t dataSize) {
	if ((getFreeJournalEntries() <= 0) || (getFreeDataSpace() < dataSize)) {
		return false;
	}
	return true;
}

bool Eeprom_at24c256::journalInfoIsValid(void) {
	const uint8_t journalInfoTypeSize = sizeof(JournalInfoType);
	JournalInfoType journalInfo;
	uint8_t *journalInfoBuff = reinterpret_cast<uint8_t*>(&journalInfo);

	_socket->readBlock(JOURNAL_INFO_ADDRESS, journalInfoBuff,
			journalInfoTypeSize);

	if (blockIsValid(journalInfoBuff, journalInfoTypeSize)) {
		if (journalInfo.version == JOURNAL_INFO_VERSION) {
			return true;
		}
	}
	return false;
}

bool Eeprom_at24c256::blockIsValid(uint8_t *buffer, uint8_t buffer_len) {
	uint8_t buffCrc = buffer[buffer_len - sizeof(CrcType)];
	uint8_t calCrc = CrcSocket::calc_chksum(buffer,
			buffer_len - sizeof(CrcType));
	return buffCrc == calCrc;
}

void Eeprom_at24c256::writeJournalInfo(void) {
	JournalInfoType journalInfo;
	journalInfo.version = JOURNAL_INFO_VERSION;
	journalInfo.journalLen = JOURNAL_SIZE;
	journalInfo.crc = CrcSocket::calc_chksum(
			reinterpret_cast<uint8_t*>(&journalInfo),
			sizeof(JournalInfoType) - 1);

	_socket->writeBlock(JOURNAL_INFO_ADDRESS,
			reinterpret_cast<uint8_t*>(&journalInfo), sizeof(JournalInfoType));
}

} // namespace eeprom

