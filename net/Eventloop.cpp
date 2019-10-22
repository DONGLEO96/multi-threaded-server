#include "Eventloop.h"
#include<iostream>
#include<sys/eventfd.h>
#include<unistd.h>
#include<sys/syscall.h>
#include"CurrentThread.h"
Eventloop::Eventloop() :_mutex(),
						_quit(false),
						_poller(new Poller(this)), 
						wakeupFd(creatEventfd()),wakeupChannel(wakeupFd,this),
						threadId(static_cast<pid_t>(::syscall(SYS_gettid))),
						_callingPendingFunctors(false),
						funcBuffer(524288)
{
	wakeupChannel.setReadCallback(std::bind(&Eventloop::handleRead, this));
	wakeupChannel.EnableReading();
}


Eventloop::~Eventloop()
{
	wakeupChannel.disableAll();
	removeChannel(wakeupFd);
	close(wakeupFd);
}
void Eventloop::loop()
{
	while (!_quit)
	{
		ActivityChannel.clear();
		_poller->poll(ActivityChannel);
		//for (std::vector<Channel*>::iterator it = ActivityChannel.begin(); it != ActivityChannel.end(); it++)
		//{
		//	(*it)->handleEvent();
		//}
		for (Channel* it : ActivityChannel)
		{
			it->handleEvent();
		}
		doPendingFunctors();
		_poller->handleExpired();
	}
}
void Eventloop::update(Channel* channel)
{
	_poller->update(channel);
}
void Eventloop::removeChannel(int fd)
{
	_poller->removeChannel(fd);
}
int Eventloop::creatEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		abort();
	}
	return evtfd;
}
void Eventloop::handleRead()
{
	uint64_t one = 1;
//	int one = 1;
	ssize_t n = read(wakeupFd, &one, sizeof(one));
//	char buf[8];
//	ssize_t n = read(wakeupFd, buf, sizeof(buf));
	//std::cout << wakeupFd << " is wakeup" << std::endl;
}
void Eventloop::wakeup()
{
	uint64_t one = 1;
	//char buf[8] = "1234567";
	//int one = 1;
	ssize_t n = write(wakeupFd, &one, sizeof(one));//发送八位有效数字
	//ssize_t n = write(wakeupFd, buf, sizeof(buf));
	//std::cout <<wakeupFd<<":"<< n << std::endl;
}
pid_t Eventloop::getThreadId()
{
	return threadId;
}
bool Eventloop::isLoopThread()
{
	return threadId == CurrentThread::tid(); //static_cast<pid_t>(::syscall(SYS_gettid));//换成CurrentTid中的函数，减少系统调用次数
}
void Eventloop::quit()//主循环不需要，后面添加的，循环池中的loop是需要退出的。
{
	_quit = true;
	if (!isLoopThread())
	{
		wakeup();
	}
}
void Eventloop::assertInLoopThread()
{
	if (!isLoopThread())
	{
		std::cout << "eventloop don't equal to current thread" << std::endl;
		exit(1);
	}
}
//void Eventloop::doPendingFunctors()
//{
//	std::vector<Functor> functors;
//	_callingPendingFunctors = true;
//
//	{
//		MutexGuard lock(_mutex);
//		functors.swap(_functorList);
//	}
//
//	for (const Functor& functor : functors)
//	{
//		functor();
//	}
//	_callingPendingFunctors = false;
//}
void Eventloop::doPendingFunctors()
{

	std::vector<Functor> functors(funcBuffer.bufferSize());
	funcBuffer.bufferOut(&*functors.begin(),functors.size());
	_callingPendingFunctors = true;
	for (const Functor functor : functors)
	{
		functor();
	}
	_callingPendingFunctors = false;
}
//void Eventloop::queueInLoop(Functor cb)
//{
//	{
//		MutexGuard lock(_mutex);
//		_functorList.push_back(std::move(cb));
//	}
//	if (!isLoopThread() || _callingPendingFunctors)//_callingPendingFunctors参数防止实在dopendingfunc函数中添加新任务，唤醒下一次poll让函数得以执行
//	{
//		wakeup();
//	}
//}
void Eventloop::queueInLoop(Functor cb)
{
	funcBuffer.bufferIn(cb);
	if (!isLoopThread() || _callingPendingFunctors)//_callingPendingFunctors参数防止实在dopendingfunc函数中添加新任务，唤醒下一次poll让函数得以执行
	{
		wakeup();
	}
}
void Eventloop::runInLoop(Functor cb)
{
	if (isLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(std::move(cb));
	}
}
void Eventloop::runAfter(int second, Timer::DeleteCallback cb)
{
	TimeStamp timeoutstamp(TimeStamp::Now());
	timeoutstamp.addTime(second);
	_poller->add_timer(timeoutstamp, cb);
}
void Eventloop::runAt(TimeStamp timeout, Timer::DeleteCallback cb)
{
	_poller->add_timer(timeout, cb);
}
void Eventloop::runEvery(int second, Timer::DeleteCallback cb)
{
	TimeStamp timeoutstamp(TimeStamp::Now());
	timeoutstamp.addTime(second);
	_poller->add_timer(timeoutstamp, second, cb);
}
