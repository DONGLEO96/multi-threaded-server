#pragma once
#include"Channel.h"
#include<functional>
#include<vector>
#include"Poller.h"
#include"Mutex.h"
#include"RingBuffer.h"
class Eventloop
{
public:
	typedef std::function<void()> Functor;
	typedef std::unique_ptr<Poller> pollerPtr;
	Eventloop();
	~Eventloop();
	void loop();
	void update(Channel* channel);
	void removeChannel(int fd);
	int creatEventfd();
	void handleRead();
	void wakeup();
	pid_t getThreadId();
	bool isLoopThread();
	void quit();
	void assertInLoopThread();
	void doPendingFunctors();
	void queueInLoop(Functor cb);
	void runInLoop(Functor cb);
	//增加定时器接口
	void runAfter(int second,Timer::DeleteCallback cb);
	void runAt(TimeStamp timeout, Timer::DeleteCallback cb);
	void runEvery(int second, Timer::DeleteCallback cb);
	//inline bool isQueueInLoop() { return funcBuffer.bufferIsFull()==false; }
private:
	Mutex _mutex;
	bool _quit;
	//Poller* _poller;//memory leak,用智能指针替换
	pollerPtr _poller;
	int wakeupFd;
	Channel wakeupChannel;
	pid_t threadId;
	std::vector<Functor> _functorList;
	bool _callingPendingFunctors;
	std::vector<Channel*> ActivityChannel;
	RingBuffer funcBuffer;


};

