#include "Poller.h"
#include"Channel.h"
#include<sys/poll.h>
#include<iostream>
#include<unistd.h>
Poller::Poller(Eventloop* loop):_loop(loop)
{
}


Poller::~Poller()
{
}
void Poller::update(Channel* channel)
{
	//std::cout <<"update:"<< channel->getSockfd()<< std::endl;
	if (channel->getIndex() == -1)
	{
		struct pollfd _pollfd;
		_pollfd.fd = channel->getSockfd();
		_pollfd.events = static_cast<short>(channel->getEvent());

		pollfdList.push_back(_pollfd);
		channel->setIndex(pollfdList.size() - 1);
		channelMap[channel->getSockfd()] = channel;
	}
	else
	{
		pollfdList[channel->getIndex()].events = static_cast<short>(channel->getEvent());
		pollfdList[channel->getIndex()].revents = 0;
		if (channel->isNoneEvent())
			pollfdList[channel->getIndex()].fd = -1 - pollfdList[channel->getIndex()].fd;
	}
}
void Poller::removeChannel(int fd)
{
	//std::cout << "close soctet" << fd << std::endl;
	Channel* channel = channelMap[fd];
	channelMap.erase(fd);
	size_t index = channel->getIndex();
	if (index == pollfdList.size() - 1)
	{
		pollfdList.pop_back();
	}
	else
	{
		int channelEndfd = pollfdList.back().fd;
		std::iter_swap(pollfdList.begin() + index, pollfdList.end() - 1);
		channelMap[channelEndfd]->setIndex(index);
		pollfdList.pop_back();
	}
	//delete channel;channel生存周期应该由connection控制,connection析构时channel自然析构

}
void Poller::poll(std::vector<Channel*>& ActivityChannel)
{
	//std::cout << " channelmap nums:" << channelMap.size() << std::endl;
	//std::cout << "pollfdslist nums:" << pollfdList.size() << std::endl;//调试用，不需要了
	SetpollTimeout();
	//----------测试代码-----------
	//std::cout << "poll:" << std:: endl;
	//for (pollfd a: pollfdList)
	//{
	//	std::cout << a.fd << std::endl<<a.events<<std::endl;
	//}
	//----------测试代码-----------
	int nready = ::poll(&*pollfdList.begin(), pollfdList.size(), _timeout);//_timeout

	if (nready > 0)
		fillActivityChannel(ActivityChannel);
}
void Poller::fillActivityChannel(std::vector<Channel*>& ActivityChannel)
{
	for (std::vector<pollfd>::iterator it = pollfdList.begin(); it != pollfdList.end(); it++)
	{
		if (it->revents > 0)
		{
			std::map<int, Channel*>::iterator ch = channelMap.find(it->fd);
			Channel* channel = ch->second;
			channel->setRevent(it->revents);
			ActivityChannel.push_back(channel);
		}
	}
}
void Poller::add_timer(TimeStamp timeout,Timer::DeleteCallback cb)
{
	_timerqueue.addTimer(timeout, cb);
}
void Poller::add_timer(TimeStamp timeout,int second, Timer::DeleteCallback cb)
{
	_timerqueue.addTimer(timeout, second, cb);
}
void Poller::SetpollTimeout()
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