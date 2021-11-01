/*
 * Nokia_3310_bitbanging.h
 *
 *  Created on: 18.09.2015
 *      Author: harald
 */

#ifndef DEVICES_PCD8544_LCD_PCD8544_SOCKET_H_
#define DEVICES_PCD8544_LCD_PCD8544_SOCKET_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"


// dat_type in function write_byte
#define PCD8544_COMMAND 0
#define PCD8544_DATA    1


class PCD8544_socket
{
public:

  typedef struct {
	GPIO_TypeDef* 	CS_port;
	uint16_t		CS_pin;
	GPIO_TypeDef*	RESET_port;
	uint16_t		RESET_pin;
	GPIO_TypeDef* 	CMD_port;
	uint16_t		CMD_pin;
	GPIO_TypeDef* 	LED_Port;
	uint16_t		LED_pin;
  } spi_pinspack_type;


  PCD8544_socket(SPI_HandleTypeDef* spi,
				 spi_pinspack_type* lcd_pinspack);

  void write_byte(uint8_t data, uint8_t command_or_data);
  void write_many_bytes(uint8_t* data, uint8_t len, uint8_t command_or_data);

  bool send_byte(uint8_t byte);
  bool send_many_bytes(uint8_t *ptr_to_data, uint8_t len, uint16_t timeout);
  bool is_TX_pending(void);

  // select device
  inline void activate_CS(void)
  {
	HAL_GPIO_WritePin(
		pinspack->CS_port, pinspack->CS_pin, GPIO_PIN_RESET);
  }

  // deselect device
  inline void deactivate_CS(void)
  {
	HAL_GPIO_WritePin(
		pinspack->CS_port, pinspack->CS_pin, GPIO_PIN_SET);
  }

  // initiate reset
  inline void pull_reset(void)
  {
	HAL_GPIO_WritePin(
		pinspack->RESET_port, pinspack->RESET_pin, GPIO_PIN_RESET);
  }

  // release reset line
  inline void release_reset(void)
  {
	HAL_GPIO_WritePin(
		pinspack->RESET_port, pinspack->RESET_pin, GPIO_PIN_SET);
  }

  inline void do_reset(void)
  {
	pull_reset();
	HAL_Delay(1);
	release_reset();
	HAL_Delay(1);
  }

  inline void command_active(void)
  {
	HAL_GPIO_WritePin(
		pinspack->CMD_port, pinspack->CMD_pin, GPIO_PIN_RESET);
  }

  inline void data_active(void)
  {
	HAL_GPIO_WritePin(
		pinspack->CMD_port, pinspack->CMD_pin, GPIO_PIN_SET);
  }

  inline void backlight_off(void)
  {
	HAL_GPIO_WritePin(
		pinspack->LED_Port, pinspack->LED_pin, GPIO_PIN_SET);
  }

  inline void backlight_on(void)
  {
	HAL_GPIO_WritePin(
		pinspack->LED_Port, pinspack->LED_pin, GPIO_PIN_RESET);
  }


private:
  SPI_HandleTypeDef* spi;
  spi_pinspack_type* pinspack;

};


#endif /* DEVICES_PCD8544_LCD_PCD8544_SOCKET_H_ */
