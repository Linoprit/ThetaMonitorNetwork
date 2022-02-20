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
#if defined STM32F401xE  || defined STM32F401xC
#include "stm32f4xx_hal.h"
#endif

#ifdef __x86_64
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#endif

class OsHelpers {
public:
	OsHelpers() {
	}
	;
	virtual ~OsHelpers() {
	}
	;
#ifdef __x86_64
	static inline void yield(void) {
		asm("NOP");
	} // do nothing.
	static inline void delay(uint32_t delay) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(delay));
	}
	static inline void SYSTEM_REBOOT() {
		asm("NOP");
	}
	static inline void SYSTEM_EXIT(){
		exit(0);
	}
	static inline uint32_t get_tick(void) { return systemTick; };
	static inline void NOP(void) {
		asm("NOP");
	}

	static void setSystemTick(uint32_t currentTick) { systemTick = currentTick; };

private:
	static uint32_t systemTick;

	OsHelpers() {
	}
	;
	virtual ~OsHelpers() {
	}
	;

#else
	// We just look, if there is any RTOS definition
#ifdef  osCMSIS
	static inline void delay(uint32_t delay) { osDelay(delay); }
	static inline void yield(void) { osThreadYield(); }
#else
	static inline void delay(uint32_t delay) {
		HAL_Delay(delay);
	}
	static inline void yield(void) {
		asm("NOP");
	}
#endif

	static inline void SYSTEM_REBOOT() {
		NVIC_SystemReset();
	}
	static inline uint32_t get_tick(void) {
		return HAL_GetTick();
	}
	;
	static inline void NOP(void) {
		asm("NOP");
	}
	static inline void SYSTEM_EXIT() {
		NVIC_SystemReset();
	}
};
#endif	// #ifdef __x86_64

#ifndef __x86_64
// We must enable OS manually
#include "cmsis_os.h"

#ifdef  osCMSIS
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void *ptr);
void operator delete(void *ptr, unsigned int i);
void operator delete[](void *ptr);
void operator delete [](void*, unsigned int i);
#endif
#endif // __x86_64

#endif /* INSTANCES_OSHELPERS_H_ */
