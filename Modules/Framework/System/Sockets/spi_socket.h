/*
 * spi_socket.h
 *
 *  Created on: 04.11.2018
 *      Author: harald
 */

#ifndef SOCKETS_SPI_SOCKET_H_
#define SOCKETS_SPI_SOCKET_H_

#ifdef __x86_64
#include "stm32f1xx_hal.h"
#elif defined STM32F401xE  || defined STM32F401xC
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include <System/serialPrintf.h>
#elif defined STM32F103xB
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"
#endif

#ifndef __x86_64
// FIXME make it working on X86
class Spi_socket {
public:
	Spi_socket() :
			hspi { nullptr } {
		// NOP
	}
	;
	Spi_socket(SPI_HandleTypeDef *hspi) {
		this->hspi = hspi;
		this->rx_buffer = 0;
	}
	;
	virtual ~Spi_socket() {
	}
	;
	void setSpi(SPI_HandleTypeDef *hspi) {
		this->hspi = hspi;
	}
	;
	uint8_t readwrite(uint8_t data) {
		// TODO blocking... use DMA?

//		  HAL_SPI_STATE_RESET      = 0x00U,    /*!< Peripheral not Initialized                         */
//		  HAL_SPI_STATE_READY      = 0x01U,    /*!< Peripheral Initialized and ready for use           */
//		  HAL_SPI_STATE_BUSY       = 0x02U,    /*!< an internal process is ongoing                     */
//		  HAL_SPI_STATE_BUSY_TX    = 0x03U,    /*!< Data Transmission process is ongoing               */
//		  HAL_SPI_STATE_BUSY_RX    = 0x04U,    /*!< Data Reception process is ongoing                  */
//		  HAL_SPI_STATE_BUSY_TX_RX = 0x05U,    /*!< Data Transmission and Reception process is ongoing */
//		  HAL_SPI_STATE_ERROR      = 0x06U,    /*!< SPI error state                                    */
//		  HAL_SPI_STATE_ABORT      = 0x07U     /*!< SPI abort is ongoing
	// HAL_SPI_ERROR_NONE              (0x00000000U)   /*!< No error                               */
	// HAL_SPI_ERROR_MODF              (0x00000001U)   /*!< MODF error                             */
	// HAL_SPI_ERROR_CRC               (0x00000002U)   /*!< CRC error                              */
	// HAL_SPI_ERROR_OVR               (0x00000004U)   /*!< OVR error                              */
	// HAL_SPI_ERROR_FRE               (0x00000008U)   /*!< FRE error                              */
	// HAL_SPI_ERROR_DMA               (0x00000010U)   /*!< DMA transfer error                     */
	// HAL_SPI_ERROR_FLAG              (0x00000020U)   /*!< Error on RXNE/TXE/BSY Flag             */
	// HAL_SPI_ERROR_ABORT             (0x00000040U)   /*!< Error during SPI Abort procedure       */
	// HAL_SPI_ERROR_INVALID_CALLBACK  (0x00000080U)   /*!< Invalid Callback error                 */

		//HAL_StatusTypeDef halState =
		// HAL_SPI_TransmitReceive(this->hspi, &data, &rx_buffer, 1, 500);

		//HAL_SPI_StateTypeDef spiState = HAL_SPI_GetState(this->hspi);
		//tx_printf("hal/spi state: %i %i\n", halState, spiState);

		HAL_SPI_TransmitReceive(this->hspi, &data, &rx_buffer, 1, 500);
		return rx_buffer;
	}
private:
	SPI_HandleTypeDef *hspi;
	uint8_t rx_buffer; // TODO reads only one byte...
};
#endif

#endif /* SOCKETS_SPI_SOCKET_H_ */
