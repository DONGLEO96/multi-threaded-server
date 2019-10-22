#include "Timer.h"



Timer::Timer(TimeStamp timeout, DeleteCallback cb):_deleted(false),_repeat(false),_expiredTime(timeout),_deleteCallback(cb),_repeatSecond(0)
{
}
Timer::Timer(TimeStamp timeout, int second, DeleteCallback cb):_deleted(false),_repeat(true), _expiredTime(timeout), 
																_deleteCallback(cb), _repeatSecond(second)
{
}
Timer::~Timer()
{
	_deleteCallback();
}
void Timer::update(TimeStamp timeout)
{
	this->_expiredTime.reset(timeout);
}
bool Timer::isValid()
{
	TimeStamp now(TimeStamp::Now());
	if (this->_expiredTime < now)
		return false;
	else
		return true;
}

void TimerQueue::addTimer(TimeStamp timeout, Timer::DeleteCallback cb)
{
	TimerPtr timer(new Timer(timeout, cb));
	_timerQueue.push(timer);
}
void TimerQueue::addTimer(TimeStamp timeout, int second, Timer::DeleteCallback cb)//周期定时使用
{
	TimerPtr timer(new Timer(timeout, second, cb));
	_timerQueue.push(timer);
}
void TimerQueue::handleExpriedEvent()
{
	while (!_timerQueue.empty())
	{
		TimerPtr currentTimer = _timerQueue.top();
/*		if (currentTimer->isDeleted())
		{
			_timerQueue.pop();好像暂时没用
		}
		else */if (!currentTimer->isValid())
		{
			if (currentTimer->isRepeated())
			{
				addTimer(currentTimer->getNextExpiredTime(), currentTimer->getRepeatSecond(), currentTimer->getDeleteCallback());
			}
			_timerQueue.pop();
		}
		else
		{
			break;
			//剩余定时器未超时
		}
	}
}