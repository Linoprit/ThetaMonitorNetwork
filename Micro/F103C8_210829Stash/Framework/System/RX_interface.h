/*
 * Comm_socket_RX_only.h
 *
 *  Created on: 29.03.2016
 *      Author: harald
 */

#ifndef SYSTEM_RX_INTERFACE_H_
#define SYSTEM_RX_INTERFACE_H_

#include <Libraries/SimpleQueue.h>


class RX_interface
{
public:
  RX_interface() {};

  virtual ~RX_interface() {};

  virtual SimpleQueue<uint8_t>* get_rx_ringbuffer() 		= 0;

};

#endif /* SYSTEM_RX_INTERFACE_H_ */
