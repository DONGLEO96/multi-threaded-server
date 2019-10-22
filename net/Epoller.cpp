#include "Epoller.h"
#include"Channel.h"
#include<unistd.h>
#include<memory.h>
#include<iostream>
#include<error.h>
#include"Eventloop.h"
#include<assert.h>
Epoller::Epoller(Eventloop* loop):epfd(epoll_create(1)),_loop(loop),epollEvents(initSize)
{
}


Epoller::~Epoller()
{
	close(epfd);
}

void Epoller::update(Channel * channel)
{
	_loop->assertInLoopThread();
	const int index = channel->getIndex();
	if (index == kNew || index == kDeleted)
	{	//新channel或者未监听的channel
		int fd = channel->getSockfd();
		if (index == kNew)
		{
			assert(channelMap.find(fd) == channelMap.cend());
			channelMap[fd] = channel;
		}
		else
		{
			assert(channelMap.find(fd) != channelMap.cend());
			assert(channelMap[fd] == channel);
		}
		channel->setIndex(kAdded);
		updateEpfd(EPOLL_CTL_ADD, channel);
	}
	else
	{
		int fd = channel->getSockfd();
		assert(channelMap.find(fd) != channelMap.cend());
		assert(channelMap[fd] == channel);
		assert(index == kAdded);
		if (channel->isNoneEvent())
		{
			updateEpfd(EPOLL_CTL_DEL, channel);
			channel->setIndex(kDeleted);
		}
		else
		{
			updateEpfd(EPOLL_CTL_MOD, channel);
		}

	}
}

void Epoller::poll(std::vector<Channel*>& ActivityChannel)
{
	SetpollTimeout();
	int eventNum = epoll_wait(epfd, &(*epollEvents.begin()), static_cast<int>(epollEvents.size()), _timeout);//添加定时器后此处为timeout值
	if (eventNum > 0)
	{
		fillActivityChannel(ActivityChannel,eventNum);
		if (static_cast<size_t>(eventNum) == epollEvents.size())
		{
			epollEvents.resize(epollEvents.size() * 2);
		}
	}
	if (eventNum < 0)
	{
		std::cout << "epoll_wait error,error=" <<error<< std::endl;
		exit(1);

	}
}

void Epoller::removeChannel(int fd)
{
	_loop->assertInLoopThread();
	Channel* channel = channelMap[fd];
	channelMap.erase(fd);
	int index = channel->getIndex();
	assert(index == kAdded || index == kDeleted);
	if (index == kAdded)
	{
		updateEpfd(EPOLL_CTL_DEL, channel);
	}
	channel->setIndex(kNew);
	
}

void Epoller::updateEpfd(int operation, Channel * channel)
{
	epoll_event event;
	memset(&event, 0, sizeof event);
	event.events = channel->getEvent();
	event.data.fd = channel->getSockfd();
	int fd = channel->getSockfd();
	if (epoll_ctl(epfd, operation, fd, &event) < 0)
	{
		std::cout << "operation error" << std::endl;
		exit(1);
	}
}

void Epoller::fillActivityChannel(std::vector<Channel*>& ActivityChannel,int eventNum)
{
	for (int i=0;i<eventNum;++i)
	{
		if (epollEvents[i].events > 0)
		{
			//Channel* channel = static_cast<Channel*>(epollEvents[i].data.ptr);
			Channel* channel = channelMap[epollEvents[i].data.fd];
			channel->setRevent(epollEvents[i].events);
			ActivityChannel.push_back(channel);
		}
	}
}
void Epoller::SetpollTimeout()
{
	TimeStamp now(TimeStamp::Now());
	if (_timerqueue.isEmpty())
	{
		_timeout = -1;
	}
	else if (_timerqueue.GetLatestTimestamp() <= now)
	{
		_timeout = 0;
	}
	else
	{
		_timeout = static_cast<int>((_timerqueue.GetLatestTimestamp().microSecondsSinceEpoch() - now.microSecondsSinceEpoch()) / 1000);
	}
}
void Epoller::add_timer(TimeStamp timeout, Timer::DeleteCallback cb)
{
	_timerqueue.addTimer(timeout, cb);
}
void Epoller::add_timer(TimeStamp timeout, int second, Timer::DeleteCallback cb)
{
	_timerqueue.addTimer(timeout, second, cb);
}
