/*
 * Measurements.h
 *
 *  Created on: 08.01.2022
 *      Author: harald
 */

#ifndef APPLICATION_SENSORS_MEASUREMENTS_H_
#define APPLICATION_SENSORS_MEASUREMENTS_H_

#include <stdint.h>
#include <Config/config.h>
#include <Application/Sensors/NonVolatileData.h>
#include <System/OsHelpers.h>
#include <array>

namespace snsrs {

// This type, must fit into the nRF-payload-struct.
// Refer to RadioMessage::PAYLOAD_LEN.
typedef struct PACKED {
	uint32_t sensorIdHash;
	float value;
	uint32_t lastUpdateTick; // in seconds
} MeasurementType;
constexpr MeasurementType IMVALID_MEASUREMENT {
		NonVolatileData::EMPTY_SENSOR_HASH,
		NAN, 0 };

template<int N>
class Measurements {
public:
	typedef std::array<MeasurementType, N> MeasurementArray;

	Measurements(osSemaphoreId_t semaphore) :
			_semaphore { semaphore } {
		for (MeasurementType &item : _measurementArray) {
			item = IMVALID_MEASUREMENT;
		}
	}

	virtual ~Measurements() {
	}

	void update(uint32_t sensorIdHash, float value) {

		if (osSemaphoreAcquire(_semaphore, 10) != osOK) {
			return;
		}
		for (MeasurementType &item : _measurementArray) {
			if ((item.sensorIdHash == sensorIdHash)
					|| (item.sensorIdHash == NonVolatileData::EMPTY_SENSOR_HASH)) {
				item.sensorIdHash = sensorIdHash;
				item.value = value;
				item.lastUpdateTick = OsHelpers::get_tick_seconds();
				break;
			}
		}
		osSemaphoreRelease(_semaphore);
	}

	const MeasurementType getMsmnt(uint32_t sensorId) {
		for (MeasurementType item : _measurementArray) {
			if (item.sensorIdHash == sensorId) {
				return item;
			}
		}
		return IMVALID_MEASUREMENT;
	}

	// returns how many sensorIdHashes are != EMPTY_SENSOR_HASH
	uint8_t getValidCount(void) {
		for (uint8_t i = 0; i < _measurementArray.size(); i++) {
			if (_measurementArray.at(i).sensorIdHash
					== NonVolatileData::EMPTY_SENSOR_HASH) {
				return i;
			}
		}
		return _measurementArray.size();
	}

	// In case of a sensor-timeout, the value is set to NAN
	static bool isValueValid(MeasurementType msmntItem) {
		return !isnan(msmntItem.value);
	}

	MeasurementArray* getArray(void) {
		return &_measurementArray;
	}

private:
	MeasurementArray _measurementArray;
	osSemaphoreId_t _semaphore;

};

typedef Measurements<MAX_SENSORS> ThetaMsmnt;

} /* namespace snsrs */

#endif /* APPLICATION_SENSORS_MEASUREMENTS_H_ */
