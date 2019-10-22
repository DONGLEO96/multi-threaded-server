#pragma once
#include<pthread.h>
#include<functional>
#include"CountLatch.h"
class Thread
{
public:
	typedef std::function<void()> ThreadFunc;
	Thread(ThreadFunc,const std::string name=std::string());
	~Thread();
	void start();
	int join();
	bool started() { return _started; }
	pid_t tid() { return _tid; }
	const std::string& Getname() { return _name; }
private:
	void SetDefaultName();
	static void* startThread(void* obj);
	bool _started;
	bool _joined;
	pthread_t _pthreadId;
	pid_t _tid;
	ThreadFunc _func;
	std::string _name;
	CountLatch _latch;//线程同步屏障，确保主线程将等待新线程数据传回后才从start函数返回
};

