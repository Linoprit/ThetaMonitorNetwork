/*
 * ComLineConfig.h
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#ifndef SYSTEM_COMMANDLINE_COMLINECONFIG_H_
#define SYSTEM_COMMANDLINE_COMLINECONFIG_H_

#include <stdint.h>
#include <array>

namespace cLine {

// configuration
constexpr uint8_t KEY_BUFFER_LEN = 64;
constexpr uint8_t COMMAND_BUFFER_LEN = 64;
constexpr uint8_t HISTORY_DEPTH = 10;

typedef std::array<uint8_t, COMMAND_BUFFER_LEN> CmdBufferType;

} /* namespace comLine */

#endif /* SYSTEM_COMMANDLINE_COMLINECONFIG_H_ */
