/*
 * dspCallbackTask.h
 *
 *  Created on: 06.07.2018
 *      Author: harald
 */

#ifndef SYSTEM_dspCallbackTask_H_
#define SYSTEM_dspCallbackTask_H_

#include "../Application/ThetaSensors/ThetaMeasurement.h"


// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

/*
EXTERNC void startMeasureTsk(void const * argument);
EXTERNC ThetaMeasurement* get_thetaMeasurement(void);
*/

#undef EXTERNC

#ifdef __cplusplus

// put cpp includes here!!




#endif // C interface
#endif /* SYSTEM_dspCallbackTask_H_ */
