/*
 * RadioLink.cpp
 *
 *  Created on: 28.11.2021
 *      Author: harald
 */

#include <Application/RadioLink/RadioLink.h>
#include <new>

namespace radioLink {

void RadioLink::init(void) {
	new (&instance()) RadioLink();

	nRF24L01_Basis.init();
}

RadioLink& RadioLink::instance(void) {
	static RadioLink radioLink;
	return radioLink;
}

RadioLink::RadioLink() {
	// NOP
}

void RadioLink::cycle() {

}

} /* namespace radioLink */
