/*
 * Messages_Base.cpp
 *
 *  Created on: 03.02.2016
 *      Author: harald
 */

#include "Messages_Base.h"


Messages_Base::msg_nRF_theta_struct Messages_Base::msg_nRF_theta;
QQueue<Messages_Base::nRF_theta_struct> Messages_Base::thetaQueue;


void Messages_Base::put_payload_to_struct(
    uint8_t *msg_buf, rcv_header_struct *rcv_header)
{
  uint8_t *pointer = 0;

  if (rcv_header->msg_class == CLASS_MEASUREMENT)
    {
      switch (rcv_header->msg_id)
        {
        case MSG_ID_THETA: // must use define!
          pointer = (uint8_t *) &Messages_Base::msg_nRF_theta.payload;
          break;

        default:
          pointer = 0;
          break;
        }
    }

  if (pointer != 0)
    {
      for (unsigned int i = 0; i < rcv_header->payload_len; i++)
        {
          *pointer = msg_buf[i + HEADER_LENGTH]; // payload is after the header         
          pointer++;
        }
    }

  if (rcv_header->msg_id == MSG_ID_THETA)
  {
      //int before = thetaQueue.length();
      thetaQueue.enqueue(Messages_Base::msg_nRF_theta.payload);

      //qDebug() << "before / after: " << before << thetaQueue.length();

      /*qDebug() << Messages_Base::msg_nRF_theta.payload.sensor_id[0]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[1]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[2]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[3]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[4]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[5]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[6]
              << Messages_Base::msg_nRF_theta.payload.sensor_id[7]
              << Messages_Base::msg_nRF_theta.payload.theta;*/
  }

}


uint8_t Messages_Base::calc_checksum(rcv_header_struct *rcv_header)
{
    uint8_t* msg_buffer = (uint8_t*) rcv_header;
    return calc_checksum(rcv_header, msg_buffer);
}


uint8_t Messages_Base::calc_checksum(rcv_header_struct *rcv_header, uint8_t* msg_buffer)
{
    // use avr algorithm
    uint8_t i, crc = 0;

    // we must exclude the chksum byte at the end!
    for (i = 0; i < HEADER_LENGTH + rcv_header->payload_len; i++)
      {
        crc = _crc_ibutton_update(crc, *msg_buffer);
        msg_buffer++;
      }
    return crc;
}


#ifdef QT_TARGET
uint8_t _crc_ibutton_update(uint8_t crc, uint8_t data)
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
#endif
