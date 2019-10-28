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
	void SetConnectionCompleteCallback(TCPserver::ConnectionCallback cb) { _connectioncompleteCallback = cb; }
	int getSockfd() { return fd; }
	void send(char* data,size_t len);
	void handleClose(int fd);
	void send(std::string data);
	void activationChannel();
	inline sockaddr_in getAddr()
	{
		return _cliaddr;
	}
	void* getContext() { return Context; }
	inline void  setContext(void* context) { Context = context; }
	inline Eventloop* getEventloop() { return _loop; }
private:
	void handleRead(int fd);
	void handleWrite(int fd);
	void sendinLoop(char* data, size_t len);
	void defaultConnectionCallback();

	int fd;
	Eventloop* _loop;
	std::shared_ptr<Channel> channel;//connection持有channel智能指针，connection析构时channel自动析构
	Buffer inBuffer;//从socket读数据
	Buffer outBuffer;
	//Buffer outBuffer//向socket写数据，后面再添加
	CloseCallback _closeCallback;
	TCPserver::MessageCallback _messageCallback;
	TCPserver::WriteCompleteCallback _writecompleteCallback;
	TCPserver::ConnectionCallback _connectioncompleteCallback;
	sockaddr_in _cliaddr;//存通信对端IP与端口
	void* Context;//服务器框架只使用这个值，不负责析构这个值，交给http层去析构
				  //因为框架不知道这个值的类型,delete不了

};

