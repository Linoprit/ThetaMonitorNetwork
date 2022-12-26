/*
 * RadioStatistics.h
 *
 *  Created on: Jan 8, 2022
 *      Author: harald
 */

#ifndef APPLICATION_RADIO_RADIOSTATISTICS_H_
#define APPLICATION_RADIO_RADIOSTATISTICS_H_

#include <Config/Types/AppTypes.h>
#include <Config/config.h>
#include <System/OsHelpers.h>
#include <array>

namespace radio {

// This type, must fit into the nRF-payload-struct.
// Refer to RadioMessage::PAYLOAD_LEN.
typedef struct PACKED {
	uint32_t stationId; // is redundant
	uint8_t relayStates; // bitfield
	uint8_t lostPkgs;
	uint8_t validSensors;
	uint8_t rxBufferOverflows;
	uint32_t lastUpdateTick; // TODO should become a timestamp later!
} RadioStatisticsType;
constexpr RadioStatisticsType INVALID_RADIO_STATISTIC { 0, 0, 0, 0, 0 };

template<int N>
class RadioStatistics {
public:
	typedef std::array<RadioStatisticsType, N> RadioStatisticsArray;

	RadioStatistics() {
		for (RadioStatisticsType &item : _radioStatisticsArray) {
			item = INVALID_RADIO_STATISTIC;
		}
	}
	virtual ~RadioStatistics() {
	}

	void update(RadioStatisticsType statistic) {
		for (RadioStatisticsType &item : _radioStatisticsArray) {
			if ((item.stationId == statistic.stationId)
					|| (item.stationId = 0)) {
				item = statistic;
				return;
			}
		}
	}

	RadioStatisticsType getStatistic(uint32_t stationId) {
		for (RadioStatisticsType item : _radioStatisticsArray) {
			if ((item.stationId == 0) || (item.stationId == stationId)) {
				return item;
			}
		}
		return INVALID_RADIO_STATISTIC;
	}

//	bool isTimedOut(uint32_t stationId) {
//		RadioStatisticsType item = getStatistic(stationId);
//		if ((item.stationId == 0)
//				|| ((item.lastUpdateTick + STATION_TIMEOUT)
//						< OsHelpers::get_tick())) {
//			return true;
//		}
//		return false;
//	}

	RadioStatisticsArray* getArray(void) {
		return &_radioStatisticsArray;
	}

private:
	RadioStatisticsArray _radioStatisticsArray;
};

typedef RadioStatistics<MAX_SLAVES> RemoteRadioStatistics;

} /* namespace gate */

#endif /* APPLICATION_RADIO_RADIOSTATISTICS_H_ */
