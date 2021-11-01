/*
 *
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */

#ifndef StartnRF24Tsk_H_
#define StartnRF24Tsk_H_

#ifdef __x86_64
// ..
#else
/**
 * Used as link between C-call from main to Start...Task().
 * Does not need to be instantiated, if no use of C++ calls to other
 * classes is done.
 */
#include "cmsis_os.h"
#endif

// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
// put C defs here

#ifdef __x86_64
EXTERNC void startnRF24Task(void );
EXTERNC void startDisplayTask(void );
EXTERNC void startMeasureTask(void);
EXTERNC void startMasterSerialTask(void);
#else
EXTERNC void startnRF24Task(void * argument);
EXTERNC void startDisplayTask(void * argument);
EXTERNC void startMeasureTask(void * argument);
EXTERNC void startMasterSerialTask(void *argument);
#endif

#undef EXTERNC
#ifdef __cplusplus
// put CPP includes here!!





#endif // C interface
#endif /* StartnRF24Tsk_H_ */
