#pragma once
#include"Channel.h"
#include<netinet/in.h>
class Acceptor
{
	typedef std::function<void(int,sockaddr_in)> newConnectionCallback;
public:
	Acceptor(Eventloop* loop);
	~Acceptor();
	int sock_bind_listen();
	void setSocketNodelay(int fd);
	void setnewConnCallback(newConnectionCallback cb) { _newConnCallback = cb; }
	void handleRead();
	void start();
private:
	//TCPserver* _tcpserver;//不再需要的变量
	Eventloop* _loop;
	int listenfd;
	Channel acceptChannel;
	newConnectionCallback _newConnCallback;
};

