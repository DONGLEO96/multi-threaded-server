#pragma once
#include<queue>
#include<assert.h>
#include"Condition.h"
template<class T>
class TaskQueue
{
public:
	TaskQueue():_mutex(),notEmpty(_mutex)
	{
	
	}
	~TaskQueue() 
	{
	
	}
	void push(T task) 
	{
		MutexGuard lock(_mutex);
		taskqueue.push(task);
		notEmpty.notify();
	}
	T take()
	{
		MutexGuard lock(_mutex);
		while (taskqueue.empty())
			notEmpty.wait();
		assert(!taskqueue.empty());
		T task(std::move(taskqueue.front()));//右值引用减少拷贝次数
		taskqueue.pop();
		return std::move(task);

	}
	size_t GetSize()
	{
		MutexGuard lock(_mutex);
		return taskqueue.size();
	}
private:
	Mutex _mutex;
	Condition notEmpty;
	std::queue<T> taskqueue;
};

