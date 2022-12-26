/*
 * GPIOSocket_Relays.h
 *
 *  Created on: Dec 26, 2022
 *      Author: harald
 */

#ifndef SOCKETS_GPIOSOCKET_RELAYS_H_
#define SOCKETS_GPIOSOCKET_RELAYS_H_

#include <Config/config.h>

class GPIOSocket_Relays {
public:
	static inline void switch_relay_on(uint8_t relayNo)
	{
		if(relayNo == 0){ // by convention, 0 means no relay
			return;
		} else if (relayNo == 1){
			HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_SET);
		} else if (relayNo == 2){
			HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_SET);
		}
		else {
			return; // channels 3 - 15 are not used, at the moment
		}
	}

	static inline void switch_relay_off(uint8_t relayNo)
	{
		if(relayNo == 0){ // by convention, 0 means no relay
			return;
		} else if (relayNo == 1){
			HAL_GPIO_WritePin(RELAY_1__GPIO_Port, RELAY_1__Pin, GPIO_PIN_RESET);
		} else if (relayNo == 2){
			HAL_GPIO_WritePin(RELAY_2__GPIO_Port, RELAY_2__Pin, GPIO_PIN_RESET);
		}
		else {
			return; // channels 3 - 15 are not used, at the moment
		}
	}

	static inline uint8_t get_relay_state(uint8_t relayNo){
		if(relayNo == 0){ // by convention, 0 means no relay
			return 0;
		} else if (relayNo == 1){
			return HAL_GPIO_ReadPin(RELAY_1__GPIO_Port, RELAY_1__Pin);
		} else if (relayNo == 2){
			return HAL_GPIO_ReadPin(RELAY_2__GPIO_Port, RELAY_2__Pin);
		}
		else {
			return 0; // channels 3 - 15 are not used, at the moment
		}
	}
};

#endif /* SOCKETS_GPIOSOCKET_RELAYS_H_ */
