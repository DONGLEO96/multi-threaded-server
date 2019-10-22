#pragma once
#include<vector>
#include<map>
#include"Timer.h"
#include<sys/epoll.h>
class Channel;
class Eventloop;

class Epoller
{
public:
	Epoller(Eventloop* loop);
	~Epoller();
	void update(Channel* channel);
	void poll(std::vector<Channel*>& ActivityChannel);
	void removeChannel(int fd);
	void add_timer(TimeStamp timeout, Timer::DeleteCallback cb);
	void add_timer(TimeStamp timeout, int second, Timer::DeleteCallback cb);
	void handleExpired() { _timerqueue.handleExpriedEvent(); }
	void SetpollTimeout();
private:
	void updateEpfd(int operation,Channel* channel);
	int epfd;
	Eventloop* _loop;
	const int initSize = 128;
	std::vector<epoll_event> epollEvents;
	std::map<int, Channel*> channelMap;
	void fillActivityChannel(std::vector<Channel*>& ActivityChannel,int eventNum);
	const int kNew = -1;
	const int kAdded = 1;
	const int kDeleted = 2;
	TimerQueue _timerqueue;
	int _timeout;//下一个定时器的时间，给epoll_wait作为超时参数

};

