#pragma once
#include"Channel.h"
#include<memory>
#include<netinet/in.h>
#include"Buffer.h"
#include"TCPserver.h"
#include<arpa/inet.h>
class Connection :public std::enable_shared_from_this<Connection>
{
	typedef std::function<void(int,Eventloop*)> CloseCallback;
public:
	Connection(int sockfd, Eventloop* loop, struct sockaddr_in cliaddr);
	~Connection();
	void SetHandleClose(CloseCallback cb);
	void SetMessageCallback(TCPserver::MessageCallback cb) { _messageCallback = cb; }
	void SetWriteCompleteCallback(TCPserver::WriteCompleteCallback cb) { _writecompleteCallback = cb; }
	int getSockfd() { return fd; }//没有send函数，暂时将sockfd传出进行输出
	void send(char* data,size_t len);
	void handleClose(int fd);
	void send(std::string data);
	void activationChannel();
	inline sockaddr_in getAddr()
	{
		return _cliaddr;
	}
private:
	void handleRead(int fd);
	void handleWrite(int fd);
	void sendinLoop(char* data, size_t len);
	void defaultConnectionCallback();

	int fd;//暂时好像也没用
	Eventloop* _loop;
	std::shared_ptr<Channel> channel;//connection持有channel智能指针，connection析构时channel自动析构
	Buffer inBuffer;//从socket读数据
	Buffer outBuffer;
	//Buffer outBuffer//向socket写数据，后面再添加
	CloseCallback _closeCallback;
	TCPserver::MessageCallback _messageCallback;
	TCPserver::WriteCompleteCallback _writecompleteCallback;
	TCPserver::ConnectionCallback _connectionCallback;
	sockaddr_in _cliaddr;//存通信对端IP与端口

};

