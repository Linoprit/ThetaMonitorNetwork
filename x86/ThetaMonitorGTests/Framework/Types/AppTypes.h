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

constexpr int SUCCESS = 0;
constexpr int FAIL = -1;


#endif /* FRAMEWORK_TYPES_APPTYPES_H_ */
