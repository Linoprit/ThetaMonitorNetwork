/*
 * StartDisplayTask.h
 *
 *  Created on: 22.12.2018
 *      Author: harald
 */

#ifndef TASKS_STARTDISPLAYTASK_H_
#define TASKS_STARTDISPLAYTASK_H_


// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void startDisplayTask(void const * argument);

#undef EXTERNC
#ifdef __cplusplus

// put cpp includes here!!



#endif // C interface
#endif /* TASKS_STARTDISPLAYTASK_H_ */
