#include "Thread.h"
#include"CurrentThread.h"
#include<assert.h>
#include<sys/syscall.h>
#include<sys/prctl.h>
#include<sys/types.h>
#include<unistd.h>
namespace CurrentThread
{
	__thread int t_cachedTid = 0;
	__thread char t_tidString[32];
	__thread int t_tidStringLength = 6;
	__thread const char* t_threadName = "default";
}
pid_t gettid()
{
	return static_cast<pid_t>(::syscall(SYS_gettid));
}
void CurrentThread::cachetid()
{
	if (t_cachedTid == 0)
	{
		t_cachedTid = gettid();
		t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d", t_cachedTid);
	}
}

struct ThreadData
{
	typedef Thread::ThreadFunc ThreadFunc;
	ThreadFunc _func;
	pid_t* _tid;
	std::string _name;
	CountLatch* _latch;
	ThreadData(ThreadFunc func,const std::string& name, pid_t* tid,CountLatch* latch) :_func(func), _tid(tid),_name(name),_latch(latch)
	{
	
	}
	void runInThread()
	{
		*_tid = CurrentThread::tid();
		_tid = NULL;
		_latch->CountDown();
		_latch = NULL;
		CurrentThread::t_threadName = _name.empty() ? "Thread" : _name.c_str();
		prctl(PR_SET_NAME, CurrentThread::t_threadName);//设置进程名字
		_func();
		CurrentThread::t_threadName = "finished";
	}

};



Thread::Thread(ThreadFunc func,const std::string name):_started(false),_joined(false)
								,_pthreadId(0),_tid(0)
								,_func(func), _name(name),_latch(1)
{
	SetDefaultName();
}
Thread::~Thread()
{
	if (_started && !_joined)
		pthread_detach(_pthreadId);
}
void Thread::start()
{
	assert(!_started);
	_started = true;
	ThreadData* data = new ThreadData(_func, _name, &_tid, &_latch);
	if (pthread_create(&_pthreadId, NULL, startThread, data))
	{
		_started = false;
		delete data;
	}
	else
	{
		_latch.wait();
		assert(_tid > 0);
	}
}
void Thread::SetDefaultName()
{
	if (_name.empty())
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "Thread");
		_name = buf;
	}
}
int Thread::join()
{
	if(_started==false)
		assert(_started);
	assert(!_joined);
	_joined = true;
	return pthread_join(_pthreadId, NULL);
}
void*  Thread::startThread(void* obj)
{
	ThreadData* data = static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
	return NULL;
}