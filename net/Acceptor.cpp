#include "Acceptor.h"
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<iostream>
#include<memory.h>
#include"TCPserver.h"
#include<netinet/tcp.h>
#include"Logging.h"
Acceptor::Acceptor(Eventloop* loop):_loop(loop),listenfd(sock_bind_listen()),acceptChannel(listenfd,_loop)
{
	acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead,this));

}


Acceptor::~Acceptor()
{
}
void Acceptor::start()
{
	acceptChannel.EnableReading();
}
int Acceptor::sock_bind_listen()
{
	int on = 1;
	int listenfd = socket(AF_INET, O_CLOEXEC | SOCK_STREAM | O_NONBLOCK, 0);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8000);
	servaddr.sin_family = AF_INET;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &on, sizeof(on));//SO_REUSEPORT调试时使用
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		std::cout << "bind error" << std::endl << errno << std::endl;
		exit(1);
	}
	std::cout << "bind" << std::endl;
	if (::listen(listenfd, SOMAXCONN) == -1)
	{
		std::cout << "listen error" << std::endl << errno << std::endl;
		exit(1);
	}
	std::cout << "listen" << std::endl;
	return listenfd;
}
void Acceptor::setSocketNodelay(int fd)
{
	int enable = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}
void Acceptor::handleRead()
{
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	//int connfd = accept4(listenfd, reinterpret_cast<struct sockaddr*>(&cliaddr), &clilen, O_NONBLOCK | O_CLOEXEC);
	//if (connfd == -1)
	//{
	//	std::cout << "accept4 error" << std::endl << errno << std::endl;
	//}
	//else
	//{

	//	if (_newConnCallback)
	//		_newConnCallback(connfd, cliaddr);
	//	/*Channel* channel = new Channel(connfd, _tcpserver);
	//	channel->EnableReading();*/
	//}
	int connfd = 0;
	if ((connfd=accept4(listenfd, (sockaddr*)(&cliaddr), &clilen, O_NONBLOCK | O_CLOEXEC))>=0)
	{
		setSocketNodelay(connfd);//¹Ø±ÕNagleËã·¨
		if (_newConnCallback)
			_newConnCallback(connfd, cliaddr);
		//LOG << "accept new connection,addr:" << inet_ntoa(cliaddr.sin_addr) << "  port:" << ntohs(cliaddr.sin_port) << "\n";
	}
	if (errno != 0)
	{
		std::cout << "accept4 error" << std::endl << errno << std::endl;
	}
}
