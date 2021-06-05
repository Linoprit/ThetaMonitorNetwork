#include "stm32f1xx.h"
#include <EepromEmulation.h>

/******* Blocking mode: Polling */

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
		uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
		uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c,
		uint8_t *pData, uint16_t Size, uint32_t Timeout) {
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData,
		uint16_t Size, uint32_t Timeout) {
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c,
		uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize,
		uint8_t *pData, uint16_t Size, uint32_t Timeout) {
	eepromEmulation_write(MemAddress, pData, Size);
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
		uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size,
		uint32_t Timeout) {
	eepromEmulation_read(MemAddress, pData, Size);
	return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c,
		uint16_t DevAddress, uint32_t Trials, uint32_t Timeout) {
	return HAL_OK;
}
