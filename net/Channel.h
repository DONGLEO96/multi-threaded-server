#pragma once
#include<functional>
#include<memory>
class TCPserver;
class Eventloop;
class Connection;
class Channel
{
	typedef std::function<void()> EventCallback;
public:
	Channel(int sockfd, Eventloop* loop);
	~Channel();
	void setReadCallback(EventCallback cb) { readCallback = cb; }
	void setWriteCallback(EventCallback cb) { writeCallback = cb; }
	void setCloseCallback(EventCallback cb) { closeCallback = cb; }
	void setRevent(int revent) { _revent = revent; }
	size_t getEvent() { return _event; }
	int getIndex() { return _index; }
	void setIndex(size_t index) { _index = index; }
	void handleEvent();
	void EnableReading();
	void disableReading();
	void EnableWriting();
	void disableWriting();
	void disableAll();
	void tie(std::shared_ptr<Connection> connptr) { _tie = connptr; isSetTie = true; }
	bool isWriting() { return _event & WriteEvent; }
	bool isNoneEvent() const { return _event == NoneEvent; }
	int getSockfd() { return sock_fd; }
private:
	static const int ReadEvent;
	static const int WriteEvent;
	static const int NoneEvent;
	void update();
	int sock_fd;
	int _event;
	int _revent;
	int _index;
	std::weak_ptr<Connection> _tie;
	EventCallback closeCallback;
	EventCallback readCallback;
	EventCallback writeCallback;
	bool isSetTie;
	Eventloop* _loop;
};

