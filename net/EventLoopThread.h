#pragma once
#include<functional>
#include"Eventloop.h"
#include"Condition.h"
#include"Thread.h"
class EventLoopThread
{
public:
	typedef std::function<void(Eventloop*)> ThreadInitCallBack;
	EventLoopThread(const ThreadInitCallBack& cb=ThreadInitCallBack(), const std::string& name=std::string());
	~EventLoopThread();
	Eventloop* startLoop();
private:
	void threadFunc();
	Eventloop* _loop;
	bool _exiting;
	Mutex _mutex;
	Thread _thread;
	Condition _cond;
	ThreadInitCallBack _callback;
};

