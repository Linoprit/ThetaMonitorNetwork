/*
 * Harald Graef, 06.03.2021
 * Lib for writing to AT24Cxx Eeprom Devices.
 * Handles the roll-over, when doing sequential writing beyond the page-limits.
 */

#include "AT24Cxxx.h"
#include <System/OsHelpers.h>
#include <algorithm>
#include <string.h>

HAL_StatusTypeDef i2c_Write(uint16_t MemAddress, uint16_t MemAddSize,
		uint8_t *pData, uint16_t Size, uint32_t Timeout) {
	return HAL_I2C_Mem_Write(AT24Cxxx::I2C_HANDLE, AT24Cxxx::DEVICE_ADDRESS,
			MemAddress, 2, pData, Size, 100);
}

HAL_StatusTypeDef i2c_Read(uint16_t MemAddress, uint16_t MemAddSize,
		uint8_t *pData, uint16_t Size, uint32_t Timeout) {
	return HAL_I2C_Mem_Read(AT24Cxxx::I2C_HANDLE, AT24Cxxx::DEVICE_ADDRESS,
			MemAddress, 2, pData, Size, 100);
}

uint16_t AT24Cxxx::pageAtAddress(uint16_t address) {
	return (address / E2PAGESIZE);
}

uint16_t AT24Cxxx::lowerPageAddress(uint16_t address) {
	return (pageAtAddress(address) * E2PAGESIZE);
}

uint16_t AT24Cxxx::upperPageAddress(uint16_t address) {
	return (lowerPageAddress(address) + E2PAGESIZE - 1);
}

uint16_t AT24Cxxx::lowerAddressFromPageNr(uint16_t pageNr) {
	return (pageNr * E2PAGESIZE);
}

uint16_t AT24Cxxx::upperAddressFromPageNr(uint16_t pageNr) {
	return (lowerAddressFromPageNr(pageNr) + E2PAGESIZE - 1);
}

/*
 * If the writing goes beyond the page, we do an initial write, up to
 * the page-end, then do cyclic writing in E2PAGESIZE bunches.
 */
void AT24Cxxx::write(uint16_t startAddr, uint8_t *data, uint32_t len) {

	uint16_t startPage = pageAtAddress(startAddr);
	uint16_t endPage = pageAtAddress(startAddr + len);

	uint16_t tempStartAddr = startAddr;
	const uint16_t endAddr = startAddr + len - 1;
	for (uint16_t page = startPage; page <= endPage; page++) {
		uint16_t tempEndAddr = std::min(endAddr, upperAddressFromPageNr(page));
		uint16_t tmpLen = tempEndAddr - tempStartAddr + 1;
		uint16_t offset = tempStartAddr - startAddr;

		// blocking write... not nice.
		i2c_Write(tempStartAddr, 2, &data[offset], tmpLen, 100);
		tempStartAddr = std::min(endAddr, lowerAddressFromPageNr(page + 1));
	}
}

void AT24Cxxx::read(uint16_t startAddr, uint8_t *data, uint32_t len) {
	i2c_Read(startAddr, 2, data, len, 100);
}

/*
 * In general this is a copy of the write-function, but we generate
 * dummy-data with 0xFF in and write it in chunks.
 */
void AT24Cxxx::erase(uint16_t startAddr, uint32_t len) {

	uint16_t startPage = pageAtAddress(startAddr);
	uint16_t endPage = pageAtAddress(startAddr + len);

	uint16_t tempStartAddr = startAddr;
	const uint16_t endAddr = startAddr + len;
	for (uint16_t page = startPage; page <= endPage; page++) {
		uint16_t tempEndAddr = std::min(endAddr, upperAddressFromPageNr(page));
		uint16_t tmpLen = tempEndAddr - tempStartAddr + 1;

		uint8_t data[tmpLen];
		memset((void*) data, 0xFF, tmpLen);

		// blocking write... not nice.
		i2c_Write(tempStartAddr, 2, data, tmpLen, 100);
		tempStartAddr = std::min(endAddr, lowerAddressFromPageNr(page + 1));
	}

}
