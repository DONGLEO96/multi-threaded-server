#include "EventLoopThreadpool.h"
#include<assert.h>
EventLoopThreadpool::EventLoopThreadpool(Eventloop* baseloop, const std::string& name) :_baseLoop(baseloop),
		_name(name), _started(false), _ThreadNum(0), _next(0)
{

}


EventLoopThreadpool::~EventLoopThreadpool()
{
}

void EventLoopThreadpool::start(const ThreadInitCallBack& cb)
{
	assert(!_started);
	_baseLoop->assertInLoopThread();
	_started = true;
	for (int i = 0; i < _ThreadNum; ++i)
	{
		char buf[_name.size() + 32];
		snprintf(buf, sizeof buf, "%s%d", _name.c_str(), i);
		EventLoopThread* t = new EventLoopThread(cb, buf);
		_threads.push_back(std::unique_ptr<EventLoopThread>(t));
		_loops.push_back(t->startLoop());
	}
	if (_ThreadNum == 0&&cb)
	{
		cb(_baseLoop);
	}
}
Eventloop* EventLoopThreadpool::getNextLoop()
{
	_baseLoop->assertInLoopThread();
	assert(_started);
	Eventloop* loop = _baseLoop;
	if (!_loops.empty())
	{
		//round-robin
		loop = _loops[_next];
		++_next;
		if (_next >= _loops.size())
		{
			_next = 0;
		}
	}
	return loop;
}
