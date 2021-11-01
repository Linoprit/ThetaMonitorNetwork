/*
 * Comm_socket.h
 *
 *  Created on: 29.03.2016
 *      Author: harald
 */

#ifndef SYSTEM_TX_INTERFACE_H_
#define SYSTEM_TX_INTERFACE_H_


class Tx_interface
{
public:
  Tx_interface() {};

  virtual ~Tx_interface() {};

  virtual bool send_byte(uint8_t byte) 						= 0;
  virtual bool send_many_bytes(
	  uint8_t *ptr_to_data, uint8_t len, uint16_t timeout) 	= 0;
  virtual bool is_TX_pending(void) 							= 0;

};

#endif /* SYSTEM_TX_INTERFACE_H_ */
