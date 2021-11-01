/*
 * Conversions.h
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_LIBRARIES_CONVERT_H_
#define FRAMEWORK_LIBRARIES_CONVERT_H_

#include <stdint.h>
#include <math.h>

struct ConvLimits {
	float scale;
	float offset;
	float minVal;
	float maxVal;
};

class Convert {
public:
	Convert(void) :
			_scale { 1.0F }, _offset { 0.0F }, _minVal { 0.0F }, _maxVal { 0.0F } {
	}
	;
	Convert(float scale, float offset, float minVal, float maxVal) :
			_scale { scale }, _offset { offset }, _minVal { minVal }, _maxVal {
					maxVal } {
	}
	;
	Convert(ConvLimits lim) :
			_scale { lim.scale }, _offset { lim.offset }, _minVal { lim.minVal }, _maxVal {
					lim.maxVal } {
	}
	;

	void setLimits(ConvLimits lim) {
		_scale = lim.scale;
		_offset = lim.offset;
		_minVal = lim.minVal;
		_maxVal = lim.maxVal;
	}

	void setLimits(float scale, float offset, float minVal, float maxVal) {
		_scale = scale;
		_offset = offset;
		_minVal = minVal;
		_maxVal = maxVal;
	}

	float int2Phys(uint32_t intVal) {
		float result = static_cast<float>(intVal) * _scale + _offset;
		return saturatePhys(result);
	}

	uint32_t phys2Int(float physVal) {
		float physValSat = saturatePhys(physVal);
		uint32_t result = static_cast<uint32_t>(roundf(
				(physValSat - _offset) / _scale));
		return result;
	}

	float saturatePhys(float physVal) {
		float result = physVal;
		if (result > _maxVal) {
			result = _maxVal;
		}
		if (result < _minVal) {
			result = _minVal;
		}
		return result;
	}

private:
	float _scale;
	float _offset;
	float _minVal;
	float _maxVal;
};

#endif /* FRAMEWORK_LIBRARIES_CONVERT_H_ */
