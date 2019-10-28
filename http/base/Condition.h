#pragma once
#include"Mutex.h"
#include<time.h>
class Condition
{
public:
	Condition(Mutex& mutex) :_mutex(mutex)
	{
		pthread_cond_init(&_con,NULL);
	}
	~Condition() 
	{
		pthread_cond_destroy(&_con);
	}
	void wait()
	{
		pthread_cond_wait(&_con, _mutex.get());
	}
	void waitForSeconds(int seconds)
	{
		timespec timeout;
		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_sec += static_cast<time_t>(seconds);
		pthread_cond_timedwait(&_con, _mutex.get(), &timeout);
	}
	void notify()
	{
		pthread_cond_signal(&_con);
	}
	void notifyAll()
	{
		pthread_cond_broadcast(&_con);
	}

private:
	Mutex& _mutex;
	pthread_cond_t _con;
};
