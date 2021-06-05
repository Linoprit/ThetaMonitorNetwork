/*
 * Error_Handler.h
 *
 *  Created on: 07.10.2019
 *      Author: harald
 */

#ifndef SYSTEM_ERROR_HANDLER_H_
#define SYSTEM_ERROR_HANDLER_H_

#include <stdint.h>
#include <iostream>

// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


class ErrorHandlerException: public std::exception {
public:
	virtual const char* what() const throw() {
		return "Error Handler Exception";
	}
};


void error_handler(const char* file, uint32_t line);


#undef EXTERNC
#ifdef __cplusplus
// put cpp includes here!!

#endif // C interface
#endif /* SYSTEM_ERROR_HANDLER_H_ */
