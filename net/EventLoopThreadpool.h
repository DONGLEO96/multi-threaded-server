#pragma once
#include<functional>
#include"EventLoopThread.h"
class EventLoopThreadpool
{
public:
	typedef std::function<void(Eventloop*)> ThreadInitCallBack;
	EventLoopThreadpool(Eventloop* baseloop,const std::string& name=std::string());
	~EventLoopThreadpool();
	void setThreadNum(int numThreads) { _ThreadNum = numThreads; };
	void start(const ThreadInitCallBack& cb= ThreadInitCallBack());
	Eventloop* getNextLoop();
	std::vector<Eventloop*> getAllLoops() { return _loops; }
	//std::vector<Eventloop*> getAllLoops();
	bool started() { return _started; };
	const std::string& getName() { return _name; };
private:
	Eventloop* _baseLoop;
	std::string _name;
	bool _started;
	int _ThreadNum;
	int _next;
	std::vector<std::unique_ptr<EventLoopThread>> _threads;
	std::vector<Eventloop*> _loops;
};

