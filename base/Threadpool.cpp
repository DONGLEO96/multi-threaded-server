#include "Threadpool.h"
#include<assert.h>

Threadpool::Threadpool(std::string name) :_mutex(), notEmpty(_mutex), _started(false), _name(name), _taskqueue()
{
}
Threadpool::~Threadpool()
{
	if (_started)
		stop();
}
void Threadpool::start(int threadNums)
{
	assert(_threads.empty());
	_threadNums = threadNums;
	_started = true;
	for (int i = 0; i < _threadNums; ++i)
	{
		char id[32];
		snprintf(id, sizeof(id), "%d", i + 1);
		_threads.emplace_back(new Thread(std::bind(&Threadpool::doTask, this), _name + id));//可以进行原地构造，调用new返回的指针构造unique_ptr对象
		_threads[i]->start();
	}
}
void Threadpool::stop()
{
	{
		MutexGuard lock(_mutex);
		_started = false;
		notEmpty.notifyAll();
	}
	for (auto& it : _threads)
		it->join();
}
void Threadpool::addTask(Threadpool::Task task)
{
	if (_threads.empty())
		task();
	else
	{
		MutexGuard lock(_mutex);
		_taskqueue.push(std::move(task));
		notEmpty.notify();
	}
}
void Threadpool::doTask()
{
	while (_started)
	{
		Task task(take());
		if (task)
		{
			task();
		}
	}
}
Threadpool::Task Threadpool::take()
{
	MutexGuard lock(_mutex);
	while (_taskqueue.empty() && _started)//用于终止线程
		notEmpty.wait();
	Task task;
	if (!_taskqueue.empty())
	{
		task = _taskqueue.front();
		_taskqueue.pop();
	}
	return task;
}
