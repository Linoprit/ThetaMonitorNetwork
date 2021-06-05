/*
 * Comm_Socket.h
 *
 *  Created on: 12.01.2016
 *      Author: harald
 */

#ifndef DRIVERS_COMM_SOCKET_COMM_SOCKET_H_
#define DRIVERS_COMM_SOCKET_COMM_SOCKET_H_

#include <stdint.h>
#include "../../Libraries/simpleRingbuffer.h"


class Comm_Socket
{
public:

  virtual bool send_byte(uint8_t byte) = 0;
  virtual bool send_many_bytes(uint8_t *ptr_to_data, uint8_t len) = 0;
  virtual bool is_TX_pending(void) = 0;
  virtual simpleRingbuffer* rx_ringbuffer(void) = 0;
};

#endif /* DRIVERS_COMM_SOCKET_COMM_SOCKET_H_ */
