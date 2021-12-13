/*
 * Messages.h
 *
 *  Created on: 10.11.2018
 *      Author: harald
 */

#ifndef RADIOLINK_MESSAGES_H_
#define RADIOLINK_MESSAGES_H_

#include <stdint.h>
#include <cstddef>
#include <Sockets/CrcSocket.h>

#define	 PACKED	__attribute__ ((packed))

class MessageBase {
public:
	enum MsgClass {
		MEASUREMENT, COMMAND, REQUEST, STATS
	};

	typedef struct PACKED {
		uint8_t sync_char[2];
		MsgClass msgClass :8;
		uint8_t msgId;
		uint32_t stationId;
		uint16_t payloadLen;
	} HeaderType;

	typedef struct PACKED {
		uint8_t checksum;
	} FooterType;

	struct MessageType;

	MessageBase() {
	}
	;
	virtual ~MessageBase() {
	}
	;
	virtual size_t getMessageLen(void) = 0;
	virtual size_t getPayloadLen(void) = 0;
	virtual MessageType* getMessagePtr(void) = 0;
	virtual void prepare(void) = 0;
	virtual bool isChkSumOk(void) = 0;
	virtual void toMessage(uint8_t *rawMessage) = 0;
	virtual uint8_t* getRawPtr(void) = 0;

private:
	static constexpr uint8_t SYNC_CHAR_0 = 170;
	static constexpr uint8_t SYNC_CHAR_1 = 85;

	uint8_t calcChkSum(void) {
		uint32_t chkSum32 = CrcSocket::calcBufferedChkSum32(getRawPtr(),
				getMessageLen() - sizeof(FooterType)); // omit the crc-field
		return static_cast<uint8_t>(chkSum32);
	}
};

// T must be a PACKED type!
template<typename T>
class MessageNew: MessageBase {
public:
	typedef struct PACKED {
		HeaderType header;
		T payload;
		FooterType footer;
	} MessageType;

	// std-ctor
	MessageNew(uint32_t stationId, MsgClass msgClass, uint8_t msgId) {
		message.header.sync_char[0] = SYNC_CHAR_0;
		message.header.sync_char[1] = SYNC_CHAR_1;
		message.header.stationId = stationId;
		message.header.msgClass = msgClass;
		message.header.msgId = msgId;
		message.header.payloadLen = getPayloadLen();
		message.footer.checksum = 0;
	}
	;
	virtual ~MessageNew() {
	}
	;
	size_t getMessageLen(void) {
		return sizeof(MessageType);
	}
	;
	size_t getPayloadLen(void) {
		return sizeof(T);
	}
	;
	MessageType* getMessagePtr(void) {
		return &message;
	}
	;
	uint8_t* getRawPtr(void) {
		return reinterpret_cast<uint8_t*>(getMessagePtr());
	}
	;
	void toMessage(uint8_t *rawMessage) {
		MessageType *message = getMessagePtr();
		for (size_t i = 0; i < getMessageLen(); i++) {
			message[i] = rawMessage[i];
		}
	}
	;
	void prepare(void) {
		message.footer.checksum = calcChkSum();
	}
	;
	bool isChkSumOk(void) {
		return calcChkSum() == message.footer.checksum;
	}
	;
private:
	MessageType message;
}
;

// old version
/*


#define MSG_ID_STATISTICS			 5
#define MSG_ID_THETA 				15

// BEGIN UART message defenitions
#define SYNC_CHAR_1        170 // DEC
#define SYNC_CHAR_2         85 // DEC
#define CHK_SUM_LENGTH       1
//#define MAX_MESSAGE_LEN     30 // Maximum bytelength of received message
#define CLASS_MEASUREMENT	 5

// C interface
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
 #define EXTERNC
 #endif

// C-definition here

#undef EXTERNC
#ifdef __cplusplus

#include <Application/ThetaSensors/ThetaMeasurement.h>

//------------ do not change ------------
// header part of serial messages
typedef struct PACKED {
	uint32_t statId :32;
	uint8_t msg_class :8;
	uint8_t msg_id :8;
	uint16_t payload_len :16;
} rcv_header_struct;
constexpr uint8_t HEADER_LENGTH = sizeof(rcv_header_struct);

#define MSG_STRUCT(payload_type)		\
    typedef struct PACKED				\
    {									\
    uint8_t             sync_char[2];	\
    rcv_header_struct 	header;			\
    payload_type 		payload;		\
    uint8_t             checksum;		\
    } msg_##payload_type;				\
    static const uint16_t MSG_##payload_type##_LEN = sizeof(msg_##payload_type);
//--------------------------------------
// END UART message definitions

class Messages {
public:

	enum msg_is_new_enum {
		statistics, thetas
	};

	// maximum byte-size
	typedef struct PACKED {
		uint8_t byte[nRF_PAYLOAD_LEN];
	} nRF_dummy_struct;

	typedef struct PACKED {
		uint8_t msg_id = MSG_ID_STATISTICS;
		uint16_t lostPkgCount;
		uint16_t retransCount;
		uint8_t relay_1;
		uint8_t relay_2;
		uint8_t padding[(nRF_PAYLOAD_LEN - 7)];
	} nRF_statistics_struct;

	// longest message
	typedef struct PACKED {
		uint8_t msg_id = MSG_ID_THETA;
		uint8_t sensor_id[8];
		float theta;
	} nRF_theta_struct; //
	MSG_STRUCT(nRF_theta_struct) // for UART communication

	Messages();
	virtual ~Messages() {
	}
	;

	// for uart
	//static void prepare_uart_message(
	//		const SensorDataType* msmnt, msg_nRF_theta_struct* msg);
	static uint8_t calc_checksum(rcv_header_struct *rcv_header);
	static uint8_t _crc_ibutton_update(uint8_t crc, uint8_t data);

	// for nRF
	static void prepare_nRF_payload(uint8_t *payload, uint8_t *data,
			uint8_t msg_type);
	void put_nRF_payload_to_struct(uint8_t *msg_buf);
	void set_msg_new(msg_is_new_enum arrived_msg);
	void set_msg_done(msg_is_new_enum arrived_msg);
	bool is_msg_new(msg_is_new_enum msg);

	nRF_statistics_struct* get_msg_statistics(void);
	nRF_theta_struct* get_msg_theta(void);

private:

	nRF_statistics_struct msg_statistics;
	nRF_theta_struct msg_theta;

	// set by put_payload_to_struct, reset by computing function
	bool new_messages[thetas];

};

#endif // C interface
*/


#endif /* RADIOLINK_MESSAGES_H_ */
