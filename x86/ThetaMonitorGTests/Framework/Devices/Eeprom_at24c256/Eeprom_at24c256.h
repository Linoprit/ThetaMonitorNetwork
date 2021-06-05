/*
 * Eeprom_at24c256.cpp
 *  Created on: 03.03.2021
 *      Author: harald
 */

#ifndef DEVICES_EEPROM_AT24C256_EEPROM_AT24C256_H_
#define DEVICES_EEPROM_AT24C256_EEPROM_AT24C256_H_

#include <stdint.h>
#include <array>
#include "Eeprom_at24c256_socket.h"

namespace eeprom {

/** Represents the IDs in the EEPROM table. You can change the names, as you like,
 * but VALUE_ID_LAST must always point to the last ID. */
enum class ValueID {
	MemVal = 0, StartVal = 1, CalibVal = 2, VALUE_ID_LAST = CalibVal
};
typedef uint8_t CrcType; // change type, if i.e. 16-bit CRC is used

class Eeprom_at24c256 {
public:
	Eeprom_at24c256(void); // CAUTION: _socket is left uninitialized
	Eeprom_at24c256(Eeprom_at24c256_socket *socket);
	virtual ~Eeprom_at24c256(void) {
	}
	;
	class JournalInfoType {
	public:
		uint32_t version;		// format version
		uint32_t journalLen;	// Max num of entries
		CrcType crc;
	}__attribute__ ((packed));

	/** Convention: entry is invalid, if startAddress, or size are zero. */
	class JournalEntryType {
	public:
		uint8_t entryID;		// the "filename"
		uint16_t startAddress; 	// where data is located
		uint8_t size;		 	// data size in bytes
		CrcType crc;
		bool isInvalid(void) {
			return startAddress == 0;
		}
	}__attribute__ ((packed));

	bool init(void); // must be called after hardware is up
	void eraseEeprom(void);
	bool readData(ValueID id, uint8_t *buffer);
	bool writeData(ValueID id, uint8_t *buffer, uint8_t size);
	// maybe user wants to know before putting data into E2
	bool isDataWritable(uint16_t dataSize); // in bytes

	uint16_t getNextDataAddress(void) {
		return _nextDataAddress;
	}
	;
	uint32_t getFreeDataSpace(void) {
		return _socket->getMaxAddress() - getNextDataAddress();
	}
	;
	uint8_t getNumOfJournalEntries(void) {
		return _numOfJournalEntries;
	}
	;
	uint16_t getFreeJournalEntries(void) {
		return JOURNAL_SIZE - getNumOfJournalEntries();
	}
	;


protected:
	class JournalEntryMap {
	public:
		JournalEntryMap(void) {
			JournalEntryType journalEntry;
			journalEntry.startAddress = 0;
			journalEntry.size = 0;
			journalEntryArray.fill(journalEntry);
		}
		JournalEntryType get(ValueID id) {
			return journalEntryArray.at(static_cast<int>(id));
		}
		;
		void put(JournalEntryType journalEntry) {
			uint8_t index = static_cast<uint8_t>(journalEntry.entryID);
			journalEntryArray[index] = journalEntry;
		}
		;

	private:
		std::array<JournalEntryType,
				static_cast<int>(ValueID::VALUE_ID_LAST) + 1> journalEntryArray;
	};

	Eeprom_at24c256_socket *_socket;
	uint16_t _nextDataAddress;
	uint8_t _numOfJournalEntries;
	JournalEntryMap _journalEntryMap;

	// Lowest address is sizeof(JournalInfoType); returns 0, if no space left;
	uint16_t calcNextJournalAddress(void);
	bool writeJournalEntry(JournalEntryType journalEntry);
	void writeJournalInfo(void);
	bool journalInfoIsValid(void);
	bool readJournalEntry(uint8_t entryNumber, JournalEntryType &journalEntry);
	bool blockIsValid(uint8_t *buffer, uint8_t buffer_len);
	void invalidateJournalEntries(void);

	static constexpr uint16_t JOURNAL_SIZE = 100;
	static constexpr uint16_t JOURNAL_INFO_ADDRESS = 0;
	static constexpr uint32_t JOURNAL_INFO_VERSION = 20201012;

};

} // namespace eeprom

#endif /* DEVICES_EEPROM_AT24C256_EEPROM_AT24C256_H_ */
