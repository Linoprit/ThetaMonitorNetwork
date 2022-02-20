/*
 * Messages.h
 *
 *  Created on: 10.11.2018
 *      Author: harald
 */

#ifndef RADIOLINK_MESSAGES_H_
#define RADIOLINK_MESSAGES_H_

#include <Application/Sensors/Sensors.h>
#include <stdint.h>
#include <cstddef>
#include <Config/config.h>
#include <Sockets/CrcSocket.h>

#define	 PACKED	__attribute__ ((packed))

// nRF24 payload length for RX. Must be set to the longest possible message.
constexpr size_t nRF_PAYLOAD_LEN = sizeof(snsrs::MeasurementType);

enum MsgClass {
	MEASUREMENT, STATISTICS, COMMAND, REQUEST
};

// The structs, that are associated with this class MUST have the size of
// nRF_PAYLOAD_LEN! And they have to be PACKED.
template<typename T>
class RadioMessage {
public:

	typedef struct PACKED {
		MsgClass msgClass :8;
		uint32_t stationId;
		uint8_t payload[nRF_PAYLOAD_LEN];
		uint8_t checksum;
	} RadioMessageType; //  6 bytes + payload

	virtual ~RadioMessage() {
	}

	T& getPayload(void) {
		return reinterpret_cast<T&>(_radioMessage.payload);
	}
	uint8_t* getMessagePtr(void) {
		return reinterpret_cast<uint8_t*>(&_radioMessage);
	}
	void prepare(MsgClass msgClass) {
		_radioMessage.stationId =
				snsrs::Sensors::instance().getNonVolatileData()->getStationId();
		_radioMessage.msgClass = msgClass;
		_radioMessage.checksum = calcChkSum(_radioMessage);
	}
	bool isChkSumOk() {
		return calcChkSum(_radioMessage) == _radioMessage.checksum;
	}

private:
	RadioMessageType _radioMessage;

	uint8_t calcChkSum(RadioMessageType radioMessage) {
		uint32_t chkSum32 = CrcSocket::calcBufferedChkSum32(
				reinterpret_cast<uint8_t*>(&radioMessage),
				sizeof(RadioMessageType) - 1); // omit the crc-field
		return static_cast<uint8_t>(chkSum32); // truncate to 1 byte
	}
};

constexpr size_t RADIO_MESSAGE_LEN =
		sizeof(RadioMessage<uint8_t>::RadioMessageType);

#endif /* RADIOLINK_MESSAGES_H_ */
