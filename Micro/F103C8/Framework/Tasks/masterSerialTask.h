/*
 * MasterSerialTask.h
 *
 *  Created on: 02.02.2019
 *      Author: harald
 */

#ifndef TASKS_MASTERSERIALTASK_H_
#define TASKS_MASTERSERIALTASK_H_


#include "stm32f1xx_hal.h"
//#include "../Application/RadioLink/Messages.h"

#define	 PACKED	__attribute__ ((packed))





// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void startMasterSerialTask(void const * argument);


#undef EXTERNC
#ifdef __cplusplus

// cpp classes and includes go here
#include <System/TX_interface.h>

class UART_TX_Socket:	 Tx_interface
{
public:
	UART_TX_Socket (UART_HandleTypeDef *huart);
	bool send_byte(uint8_t byte);
	bool send_many_bytes(uint8_t *ptr_to_data, uint8_t len, uint16_t timeout=10);
	bool is_TX_pending(void);

private:
	UART_HandleTypeDef 	*huart;
};

#endif // C interface
#endif /* TASKS_MASTERSERIALTASK_H_ */
