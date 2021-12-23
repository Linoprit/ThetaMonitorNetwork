/*
 * Nokia_3310_bitbanging.h
 *
 *  Created on: 18.09.2015
 *      Author: harald
 */

#ifndef DEVICES_PCD8544_LCD_PCD8544_SOCKET_H_
#define DEVICES_PCD8544_LCD_PCD8544_SOCKET_H_

#ifndef __x86_64
// FIXME make it working on X86

#include <Sockets/GPIOSocket_PCD8544.h>
#include <System/OsHelpers.h>

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

// dat_type in function write_byte
#define PCD8544_COMMAND 0
#define PCD8544_DATA    1

namespace lcd {
class PCD8544_socket {
public:
	// convenience, so that we have only one interface
	static inline void activate_CS(void) {
		GPIOSocket_PCD8544::activate_CS();
	}
	static inline void deactivate_CS(void) {
		GPIOSocket_PCD8544::deactivate_CS();
	}
	static inline void pull_reset(void) {
		GPIOSocket_PCD8544::pull_reset();
	}
	static inline void release_reset(void) {
		GPIOSocket_PCD8544::release_reset();
	}
	static inline void command_active(void) {
		GPIOSocket_PCD8544::command_active();
	}
	static inline void data_active(void) {
		GPIOSocket_PCD8544::data_active();
	}
	static inline void backlight_off(void) {
		GPIOSocket_PCD8544::backlight_off();
	}
	static inline void backlight_on(void) {
		GPIOSocket_PCD8544::backlight_on();
	}

	static void write_byte(uint8_t data, uint8_t command_or_data) {
		activate_CS();
		if (command_or_data == PCD8544_COMMAND) {
			command_active();
		} else {
			data_active();
		}
		send_byte(data, 1);
		GPIOSocket_PCD8544::deactivate_CS();
	}
	static void write_many_bytes(uint8_t *data, uint8_t len, uint8_t command_or_data) {
		activate_CS();
		if (command_or_data == PCD8544_COMMAND) {
			GPIOSocket_PCD8544::command_active();
		} else {
			GPIOSocket_PCD8544::data_active();
		}
		send_many_bytes(data, len, 1);
		deactivate_CS();
	}
	static bool send_byte(uint8_t byte, uint16_t timeout) {
		while (HAL_SPI_GetState(&lcd_SPI) != HAL_SPI_STATE_READY) {
			OsHelpers::delay(1);
			timeout--;
			if (timeout == 0)
				return false;
		}
		//HAL_SPI_Transmit(&lcd_SPI, &byte, 1, 1);
		//HAL_SPI_Transmit_IT(&lcd_SPI, &byte, 1);
		HAL_SPI_Transmit_DMA(&lcd_SPI, &byte, 1);
		return true;
	}
	static bool send_many_bytes(uint8_t *ptr_to_data, uint8_t len, uint16_t timeout) {
		while (HAL_SPI_GetState(&lcd_SPI) != HAL_SPI_STATE_READY) {
			OsHelpers::delay(1);
			timeout--;
			if (timeout == 0)
				return false;
		}
		//HAL_SPI_Transmit(&lcd_SPI, ptr_to_data, len, 1);
		//HAL_SPI_Transmit_IT(&lcd_SPI, ptr_to_data, len);
		HAL_SPI_Transmit_DMA(&lcd_SPI, ptr_to_data, len);
		return true;
	}
	static inline void do_reset(void) {
		activate_CS();
		pull_reset();
		OsHelpers::delay(1);
		release_reset();
		OsHelpers::delay(1);
		deactivate_CS();
	}

private:
	PCD8544_socket() {
	}
	virtual ~PCD8544_socket() {
	}
};

} //namespace lcd

#endif
#endif /* DEVICES_PCD8544_LCD_PCD8544_SOCKET_H_ */
