/*
 * AppTypes.h
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_TYPES_APPTYPES_H_
#define FRAMEWORK_TYPES_APPTYPES_H_

#include <stdint.h>

enum ErrorCode {ERR_OK = 0, ERR_NOK = 1, ERR_OK_E2_NOT_WRITTEN};
// ERR_OK_E2_NOT_WRITTEN: returned by NonVoaltileData::write, if data should be written,
// 		that's the same as already in the E2PROM.

constexpr int _SUCCESS_ = 0;
constexpr int _FAIL_ = -1;

#ifndef __x86_64
# define INT8_WIDTH 8
# define UINT8_WIDTH 8
# define INT16_WIDTH 16
# define UINT16_WIDTH 16
# define INT32_WIDTH 32
# define UINT32_WIDTH 32
# define INT64_WIDTH 64
# define UINT64_WIDTH 64
#endif

#endif /* FRAMEWORK_TYPES_APPTYPES_H_ */
