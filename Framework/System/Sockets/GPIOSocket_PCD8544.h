/*
 * GPIOSocket_PCD8544.h
 *
 *  Created on: Dec 21, 2021
 *      Author: harald
 */

#ifndef SOCKETS_GPIOSOCKET_PCD8544_H_
#define SOCKETS_GPIOSOCKET_PCD8544_H_

#include <Config/config.h>

class GPIOSocket_PCD8544 {
public:
	static inline uint16_t get_IRQ_Pin(void) {
		return BUTTON_1_Pin;
	}

	// select device
	static inline void activate_CS(void) {
		HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, GPIO_PIN_RESET);
	}

	// deselect device
	static inline void deactivate_CS(void) {
		HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, GPIO_PIN_SET);
	}

	// initiate reset
	static inline void pull_reset(void) {
		HAL_GPIO_WritePin(LCD_Reset_GPIO_Port, LCD_Reset_Pin, GPIO_PIN_RESET);
	}

	// release reset line
	static inline void release_reset(void) {
		HAL_GPIO_WritePin(LCD_Reset_GPIO_Port, LCD_Reset_Pin, GPIO_PIN_SET);
	}
	static inline void command_active(void) {
		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
	}

	static inline void data_active(void) {
		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	}

	static inline void backlight_off(void) {
		HAL_GPIO_WritePin(LCD_BCKLT_GPIO_Port, LCD_BCKLT_Pin, GPIO_PIN_SET);
	}

	static inline void backlight_on(void) {
		HAL_GPIO_WritePin(LCD_BCKLT_GPIO_Port, LCD_BCKLT_Pin, GPIO_PIN_RESET);
	}
};

#endif /* SOCKETS_GPIOSOCKET_PCD8544_H_ */
