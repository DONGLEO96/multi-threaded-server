#pragma once
#include"Condition.h"
class CountLatch
{
public:
	CountLatch(int count) :_latch(count), _mutex(), _cond(_mutex) {};
	~CountLatch() {};
	void wait()
	{
		MutexGuard lock(_mutex);
		while (_latch != 0)
			_cond.wait();
	}
	void CountDown()
	{
		MutexGuard lock(_mutex);
		_latch -= 1;
		if (_latch == 0)
			_cond.notifyAll();
	}
private:
	int _latch;
	Mutex _mutex;
	Condition _cond;
};