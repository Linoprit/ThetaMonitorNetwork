/*
 * OsHelpers.cpp
 *
 *  Created on: 26.04.2020
 *      Author: harald
 */

#include <System/OsHelpers.h>
#ifdef __x86_64
	uint32_t OsHelpers::systemTick = 0;
#else

	uint32_t OsHelpers::systemTickSeconds = 0;

void * operator new( size_t size ) {
    return pvPortMalloc(size);
}

void * operator new[]( size_t size ) {
    return pvPortMalloc( size );
}

void operator delete(void * ptr, unsigned int i) {
	UNUSED(i);
  vPortFree( ptr );
}

void operator delete( void * ptr ) {
    vPortFree( ptr );
}

void operator delete [](void * ptr, unsigned int i) {
	UNUSED(i);
  vPortFree( ptr );
}


void operator delete[]( void * ptr ) {
    vPortFree( ptr );
}
#endif
