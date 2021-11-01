/*
 * SimpleQueue.h
 * taken from:
 * http://sourcetricks.com/2008/07/c-queues.html#.W4T0NWbwCHs
 *
 *  Created on: 13.05.2019
 *      Author: harald
 */

#ifndef SIMPLEQUEUE_H_
#define SIMPLEQUEUE_H_

#include <cstdlib>
#include <System/Error_Handler.h>


template <class T, std::size_t _Nm = 1> class SimpleQueue {
public:
	SimpleQueue(void) {
		_data 	= new T[_Nm];
		_maxSize = _Nm;
		reset();
	};

	SimpleQueue(std::size_t maxSize) {
		_data 	= new T[maxSize];
		_maxSize = maxSize;
		reset();
	};

	virtual ~SimpleQueue(void) {
		delete[] _data;
	};

	inline void enqueue(T element) { // put element to back of queue
		if (size() == static_cast<int16_t>(_maxSize) ) {
			error_handler(__FILE__, __LINE__ );
		}

		_size++;
		increment(_rear);
		_data[_rear] = element;
	};

	inline T dequeue(void) { // return element and remove it from queue
		if ( isEmpty() ) {
			error_handler(__FILE__, __LINE__ );
		}

		_size--;
		T ret = _data[_front];
		increment(_front);
		return ret;
	};

	void asArray(T* data) { // returns all elements as array, w/o dequeueing
		int16_t lclFront = _front;
		for (uint8_t i=0; i < _size; i++) {
			data[i] = _data[lclFront];
			increment(lclFront);
		}
	}

	void reset(void) {
		_front 	= 0;
		_rear 	= -1;
		_size   = 0;
	}

	T front(void) { // look at first element w/o dequeueing
		if ( isEmpty() ) {
			error_handler(__FILE__, __LINE__ );
		}

		return _data[_front];
	};

	int16_t size(void) { return _size;	};
	bool isEmpty(void) { return (_size == 0); };
	bool isFull(void)  { return (static_cast<std::size_t>(_size) == _maxSize); };

	int16_t getFront() { return _front;	};
	int16_t getRear()  { return _rear; 	};

private:
	T* _data;
	int16_t _front;
	int16_t _rear;
	int16_t _size;
	std::size_t _maxSize;

	void increment(int16_t &x) {
		if (x == static_cast<int16_t>(_maxSize) - 1)
			x = 0;
		else
			x++;
	};
};

#endif /* SIMPLEQUEUE_H_ */
