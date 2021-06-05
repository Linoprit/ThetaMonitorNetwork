/*
 *
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */

#ifndef StartnRF24Tsk_H_
#define StartnRF24Tsk_H_

/**
 * Used as link between C-call from main to Start...Task().
 * Does not need to be instantiated, if no use of C++ calls to other
 * classes is done.
 */


#include "../Application/ThetaSensors/ThetaMeasurement.h"
#include "cmsis_os.h"


// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif



EXTERNC void startnRF24Tsk(void const * argument);
EXTERNC ThetaMeasurement* get_remoteMsmt(void) ;



#undef EXTERNC
#ifdef __cplusplus

// put cpp includes here!!
osPoolId* get_msg_pool(void);
osMessageQId* get_queue(void);





#endif // C interface
#endif /* StartnRF24Tsk_H_ */
