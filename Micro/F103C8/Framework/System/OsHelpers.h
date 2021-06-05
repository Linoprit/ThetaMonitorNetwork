/*
 * OsHelpers.h
 *
 *  Created on: 26.04.2020
 *      Author: harald
 */

#ifndef INSTANCES_OSHELPERS_H_
#define INSTANCES_OSHELPERS_H_

#ifdef STM32F3xx
#include "stm32f3xx.h"
#endif
#ifdef STM32F303xE
#include "stm32f3xx.h"
#endif
#ifdef STM32F103xB
#include "stm32f1xx.h"
#endif
#ifdef STM32F407xx
#include "stm32f4xx.h"
#endif

#ifdef __x86_64

class OsHelpers {
public:

	static inline void yield(void) { asm("NOP"); } // do nothing.
	// TODO static inline void delay(uint32_t delay) { HAL_Delay(delay); }
	//TODO static inline void SYSTEM_REBOOT() {     NVIC_SystemReset();	}
	// TODO static inline uint32_t get_tick(void) { return HAL_GetTick(); };
	static inline void NOP(void)	{ asm("NOP"); }

private:
	OsHelpers() {};
	virtual ~OsHelpers() {};
};

#else
// We must enable OS manually
#include "cmsis_os.h"

#ifdef  osCMSIS
void * operator new( size_t size );
void * operator new[]( size_t size );
void operator delete( void * ptr );
void operator delete(void * ptr, unsigned int i);
void operator delete[]( void * ptr );
void operator delete [](void * , unsigned int i);
#endif

class OsHelpers {
public:
	OsHelpers() {};
	virtual ~OsHelpers() {};

	// We just look, if there is any RTOS definition
#ifdef  osCMSIS
	static inline void delay(uint32_t delay) { osDelay(delay); }
	static inline void yield(void) { osThreadYield(); }
#else
	static inline void delay(uint32_t delay) { HAL_Delay(delay); }
	static inline void yield(void) { asm("NOP"); }
#endif

	static inline void SYSTEM_REBOOT() {     NVIC_SystemReset();	}
	static inline uint32_t get_tick(void) { return HAL_GetTick(); };
	static inline void NOP(void)	{ asm("NOP"); }

};

#endif //  __x86_64

#endif /* INSTANCES_OSHELPERS_H_ */
