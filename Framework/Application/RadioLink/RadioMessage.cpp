/*
 * Messages.cpp
 *
 *  Created on: 10.11.2018
 *      Author: harald
 */

#include <Application/RadioLink/RadioMessage.h>
#include "cmsis_os.h"

/*
Messages::Messages()
{
	for (uint8_t i=0; i < Messages::thetas; i++)
		new_messages[i] = false;
}


// At the moment we only send the thea_struct, so there's no need to hav a 'msg_type'
// argument.
// void Messages::prepare_uart_message(
// 		const SensorDataType* msmnt, msg_nRF_theta_struct* msg)
// {
// 	msg->sync_char[0]       = SYNC_CHAR_1;
// 	msg->sync_char[1]       = SYNC_CHAR_2;
// 	msg->header.msg_class   = CLASS_MEASUREMENT;
// 	msg->header.msg_id      = MSG_ID_THETA;
// 	msg->header.payload_len = sizeof(nRF_theta_struct);
//
// 	for(uint8_t i=0; i < 8; i++)
// 	{
// 		msg->payload.sensor_id[i] = msmnt->sensor_ID[i];
// 	}
// 	msg->payload.theta	  = msmnt->temperature;
// 	msg->checksum 		  = calc_checksum(&msg->header);
// }


uint8_t Messages::calc_checksum(rcv_header_struct *rcv_header)
{
	uint8_t i, crc = 0;
	uint8_t* msg_buffer = (uint8_t*) rcv_header;

	// we must exclude the chksum byte at the end!
	uint8_t len = HEADER_LENGTH + rcv_header->payload_len;
	for (i = 0; i < len; i++)
	{
		crc = _crc_ibutton_update(crc, *msg_buffer);
		msg_buffer++;
	}
	return crc;
}


uint8_t Messages::_crc_ibutton_update(uint8_t crc, uint8_t data)
{
	uint8_t i;

	crc = crc ^ data;
	for (i = 0; i < 8; i++)
	{
		if (crc & 0x01)
			crc = (crc >> 1) ^ 0x8C;
		else
			crc >>= 1;
	}

	return crc;
}

void Messages::prepare_nRF_payload(uint8_t* payload, uint8_t* data, uint8_t msg_type)
{
	if (msg_type == MSG_ID_STATISTICS)
	{
		payload[0] = MSG_ID_STATISTICS;
	}
	else if (msg_type == MSG_ID_THETA)
	{
		payload[0] = MSG_ID_THETA;
	}

	for(uint8_t i=1; i < nRF_PAYLOAD_LEN; i++)
	{
		payload[i] = data[i-1];
	}

}

void Messages::put_nRF_payload_to_struct(uint8_t* msg_buf)
{
	uint8_t *pointer = 0;

	switch (msg_buf[0])
	{
	case nRF_CMD_SWITCH_CHANNEL:
		pointer = (uint8_t *) &cmd_switch_channel;
		set_msg_new(switch_channel);
		break;

	case nRF_CMD_USE_ADDRESS:
		pointer = (uint8_t *) &cmd_use_address;
		set_msg_new(use_address);
		break;

	case nRF_MSG_PING:
		pointer = (uint8_t *) &msg_ping;
		set_msg_new(ping);
		break;

	case nRF_MSG_STATISTICS:
		pointer = (uint8_t *) &msg_statistics;
		set_msg_new(statistics);
		break;

	case nRF_MSG_THETA:
		pointer = (uint8_t *) &msg_theta;
		set_msg_new(thetas);
		break;

	default:
		pointer = 0;
		break;
	}

	if (pointer != 0)
	{
		for (unsigned int i = 0; i < nRF_PAYLOAD_LEN; i++)
		{
			*pointer = msg_buf[i];
			pointer++;
		}
	}

	// notify the corresponding task, that data has arrived
	// osSignalSet (get_nRF24Task(), 0);

}




Messages::nRF_statistics_struct* Messages::get_msg_statistics(void)
{
	return &msg_statistics;
}

Messages::nRF_theta_struct*	Messages::get_msg_theta(void)
{
	return &msg_theta;
}

void Messages::set_msg_new(msg_is_new_enum arrived_msg)
{
	new_messages[arrived_msg] = true;
}

void Messages::set_msg_done(msg_is_new_enum arrived_msg)
{
	new_messages[arrived_msg] = false;
}

bool Messages::is_msg_new(msg_is_new_enum msg)
{
	return (new_messages[msg]);
}
*/
