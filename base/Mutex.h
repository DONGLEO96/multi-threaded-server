#pragma once
#include<pthread.h>
#include<iostream>
#include<sys/syscall.h>
#include<unistd.h>
class Mutex
{
public:
	Mutex() { pthread_mutex_init(&_mutex, NULL); }
	~Mutex() 
	{
		pthread_mutex_lock(&_mutex);
		pthread_mutex_destroy(&_mutex);
	}
	void lock()
	{
		pthread_mutex_lock(&_mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&_mutex);
	}
	pthread_mutex_t* get() { return &_mutex; }
private:
	pthread_mutex_t _mutex;
	friend class Condition;
};


class MutexGuard
{
public:
	explicit MutexGuard(Mutex& mutex) :_mutex(mutex)
	{
		_mutex.lock();

	}
	~MutexGuard()
	{
		_mutex.unlock();
	}

private:
	Mutex& _mutex;//注意，持有引用
};





