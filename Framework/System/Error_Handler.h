/*
 * Error_Handler.h
 *
 *  Created on: 07.10.2019
 *      Author: harald
 */

#ifndef SYSTEM_ERROR_HANDLER_H_
#define SYSTEM_ERROR_HANDLER_H_

#include <stdint.h>



// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif



void error_handler(const char* file, uint32_t line);


#undef EXTERNC
#ifdef __cplusplus
// put cpp includes here!!

#endif // C interface
/*
 for plantUml we declare this file as a class
@startuml Error_Handler_man.png
class Error_Handler {
 	 void error_handler(const char* file, uint32_t line)
}
@enduml
*/
#endif /* SYSTEM_ERROR_HANDLER_H_ */
