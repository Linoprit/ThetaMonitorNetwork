/*
 * HelpersLib.h
 *
 *  Created on: 26.10.2015
 *      Author: harald
 */

#ifndef LIBRARIES_HELPERSLIB_H_
#define LIBRARIES_HELPERSLIB_H_

#include <stdint.h>
#include <string>

// value2char: char* len, that holds long type result
constexpr uint8_t MAX_BUFFER_LEN = 12;

// value2char: Base
constexpr uint8_t BIN_RADIX = 2 ;
constexpr uint8_t DEC_RADIX = 10;
constexpr uint8_t HEX_RADIX = 16;

constexpr uint8_t _SUCCESS = 0;
constexpr uint8_t _BUSY = 1;
constexpr uint8_t _ERROR = 2;


class HelpersLib
{
public:
  HelpersLib ();
  static uint8_t findFirstSetBitFromRight(uint8_t inByte);
  static uint8_t findFirstSetBitFromLeft(uint8_t inByte);
  static std::string floatToStr(float value, uint8_t decimalPlace);
  static void value2char(
      char* result_buff, uint8_t result_buff_len,
      uint8_t exp, long int value);
  static uint16_t swapBytes(uint16_t value);
  static uint8_t hibyte(uint16_t value);
  static uint8_t lobyte(uint16_t value);
};

#endif /* LIBRARIES_HELPERSLIB_H_ */
