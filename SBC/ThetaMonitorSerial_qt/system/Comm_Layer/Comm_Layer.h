/*
 * Comm_Layer.h
 *
 *  Created on: 11.01.2016
 *      Author: harald
 */

#ifndef SYSTEM_COMM_LAYER_H_
#define SYSTEM_COMM_LAYER_H_


#include <stdint.h>
#include "../../drivers/Comm_Socket/Comm_Socket.h"
#include "../../Libraries/simpleRingbuffer.h"
#include "Messages_Base.h"



#define INLINE __attribute__((always_inline))
#define	 PACKED	__attribute__ ((packed))

#define TX_QUEUE_SIZE (5 * sizeof(tx_message_type))
#define MAXLOOP 100 //max count inspect_received_data() is called

class Comm_Layer
{

public:
    struct tx_message_type
    {
        uint8_t*  ptr_to_message;
        uint16_t  msg_length;
    };

    Comm_Layer (Comm_Socket *comm_socket);

    void    loop(void);
    uint8_t inspect_received_data(void);
    bool    pack_n_queue_msg(rcv_header_struct *tx_header);

    uint8_t compare_chksum_ibutton(uint8_t* msg_buffer, rcv_header_struct* rcv_header);


private:
    rcv_header_struct rcv_header;
    uint8_t           *rcv_header_ptr;
    simpleRingbuffer  *tx_queue;

    bool get_next_tx_message(tx_message_type *tx_message_ptr);
    void send_from_queue(void);

private:
    Comm_Socket *comm_socket;

};

#endif /* SYSTEM_COMM_LAYER_H_ */
