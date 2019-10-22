#pragma once
#include<vector>
#include<map>
#include<memory>
#include"Timer.h"
class Channel;
class Eventloop;
class Poller
{
public:
	Poller(Eventloop* loop);
	~Poller();
	void update(Channel* channel);
	void poll(std::vector<Channel*>& ActivityChannel);
	void removeChannel(int fd);
	void add_timer(TimeStamp timeout, Timer::DeleteCallback cb);
	void add_timer(TimeStamp timeout, int second, Timer::DeleteCallback cb);
	void handleExpired() { _timerqueue.handleExpriedEvent(); }
	void SetpollTimeout();
private:
	std::vector<struct pollfd> pollfdList;
	std::map<int, Channel*>  channelMap;
	Eventloop* _loop;//该参数暂时无用
	void fillActivityChannel(std::vector<Channel*>& ActivityChannel);
	TimerQueue _timerqueue;
	int _timeout=-1;
};

