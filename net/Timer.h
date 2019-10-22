#pragma once
#include"TimeStamp.h"
#include<functional>
#include<queue>
#include<memory>
class Timer
{

public:
	typedef std::function<void()> DeleteCallback;
	Timer(TimeStamp timeout,DeleteCallback cb);
	Timer(TimeStamp timeout, int second, DeleteCallback cb);
	~Timer();
	void SetDelete() { _deleted = true; }
	bool isValid();
	void update(TimeStamp timeout);//好像暂时没用
	bool isDeleted() { return _deleted; }//好像暂时没用
	bool isRepeated() { return _repeat; }//周期定时器使用
	int getRepeatSecond() { return _repeatSecond; }//周期定时器使用
	DeleteCallback getDeleteCallback() { return _deleteCallback; }//周期定时器使用
	TimeStamp getExpiredTime() { return _expiredTime; }
	TimeStamp getNextExpiredTime() { return TimeStamp::addTime(TimeStamp::Now(), _repeatSecond); }//周期定时使用
private:
	bool _deleted;
	bool _repeat;
	TimeStamp _expiredTime;
	DeleteCallback _deleteCallback;//析构函数中执行回调安全吗？
	int _repeatSecond;
};

struct TimerCmp
{
	bool operator()(std::shared_ptr<Timer>& a, std::shared_ptr<Timer>& b) const
	{
		return a->getExpiredTime() > b->getExpiredTime();
	}
};

class TimerQueue
{
public:
	TimerQueue() {};
	~TimerQueue() {};
	typedef std::shared_ptr<Timer> TimerPtr;
	void addTimer(TimeStamp timeout,Timer::DeleteCallback cb);
	void addTimer(TimeStamp timeout, int second, Timer::DeleteCallback cb);
	void handleExpriedEvent();
	TimeStamp GetLatestTimestamp() { return _timerQueue.top()->getExpiredTime(); }
	bool isEmpty() { return _timerQueue.empty(); }
private:
	std::priority_queue < TimerPtr, std::vector<TimerPtr>, TimerCmp> _timerQueue;
};
//Next Version:缺乏删除定时器功能
//定时器精度不够，只能到毫秒级
//考虑使用timerfd重构。。。。有时间再说吧。。。