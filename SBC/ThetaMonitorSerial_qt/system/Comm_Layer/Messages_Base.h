/*
 * Messages_Base.h
 *
 *  Created on: 03.02.2016
 *      Author: harald
 */

#ifndef SYSTEM_COMM_LAYER_MESSAGES_BASE_H_
#define SYSTEM_COMM_LAYER_MESSAGES_BASE_H_

#include <stdint.h>
#include <QQueue>
#include <QDebug>



#define	 PACKED	__attribute__ ((packed))

#define nRF_PAYLOAD_LEN				13 // nRF24 payload length for RX and TX

#define MSG_ID_STATISTICS			 5
#define MSG_ID_THETA 				15
#define DS1820_ADDRESS_LEN           8

// BEGIN UART message defenitions
#define SYNC_CHAR_1        170 // DEC
#define SYNC_CHAR_2         85 // DEC
#define CHK_SUM_LENGTH       1
#define MAX_MESSAGE_LEN     30 // Maximum bytelength of received message
#define CLASS_MEASUREMENT	 5

//------------ do not change ------------
// header part of serial messages
typedef struct PACKED
{
    uint8_t  msg_class      : 8;
    uint8_t  msg_id         : 8;
    uint16_t payload_len    : 16;
} rcv_header_struct;
const uint8_t HEADER_LENGTH = sizeof(rcv_header_struct);

#define MSG_STRUCT(payload_type)		\
    typedef struct PACKED				\
{                                       \
    uint8_t             sync_char[2];	\
    rcv_header_struct 	header;			\
    payload_type 		payload;		\
    uint8_t             checksum;		\
    } msg_##payload_type;				\
    static const uint16_t MSG_##payload_type##_LEN = sizeof(msg_##payload_type)
//--------------------------------------
// END UART message definitions

#ifdef QT_TARGET
uint8_t _crc_ibutton_update(uint8_t crc, uint8_t data);
#endif


class Messages_Base
{
public:

    enum msg_is_new_enum { statistics, thetas };

    // maximum byte-size
    typedef struct PACKED
    {
        uint8_t   byte[nRF_PAYLOAD_LEN];
    } nRF_dummy_struct;

    typedef struct PACKED
    {
        uint8_t   msg_id = MSG_ID_STATISTICS;
        uint16_t  lostPkgCount;
        uint16_t  retransCount;
        uint8_t   relay_1;
        uint8_t	  relay_2;
        uint8_t   padding[ (nRF_PAYLOAD_LEN - 7) ];
    } nRF_statistics_struct;

    // longest message
    typedef struct PACKED
    {
        uint8_t   msg_id = MSG_ID_THETA;
        uint8_t   sensor_id[DS1820_ADDRESS_LEN];
        float     theta;
    } nRF_theta_struct;
    MSG_STRUCT(nRF_theta_struct); // for UART communication
    static msg_nRF_theta_struct msg_nRF_theta;

    Messages_Base();
    static void put_payload_to_struct(uint8_t *msg_buf, rcv_header_struct *rcv_header);
    static uint8_t calc_checksum(rcv_header_struct *rcv_header, uint8_t *msg_buffer);
    static uint8_t calc_checksum(rcv_header_struct *rcv_header);
    //static QQueue<nRF_theta_struct> getThetaQueue(void) {return thetaQueue;}
    static QQueue<nRF_theta_struct>* getThetaQueue(void) {return &thetaQueue;}

private:
    //static QQueue<nRF_theta_struct> thetaQueue;
    static QQueue<nRF_theta_struct> thetaQueue;
};

#endif /* SYSTEM_COMM_LAYER_MESSAGES_BASE_H_ */
