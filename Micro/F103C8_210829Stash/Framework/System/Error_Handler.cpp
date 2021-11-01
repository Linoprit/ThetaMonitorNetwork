/*
 * Error_Handler.cpp
 *
 *  Created on: 07.10.2019
 *      Author: harald
 */

#include <System/Error_Handler.h>
#include <System/printf/printf.h>

void error_handler(const char* file, uint32_t line)
{
	printf_("Exception in %s, line: %ld\n", file, line);

	while (1) { }; // infinite loop
}

