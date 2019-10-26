#include "TCPserver.h"
#include<iostream>
#include<functional>
#include"Connection.h"
#include"Logging.h"
#include"util.h"
#include<signal.h>

void handleSIGPIPE()
{
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	if (sigaction(SIGPIPE, &sa, NULL))
		return;
}

TCPserver::TCPserver(Eventloop* loop,int threadNum) :_loop(loop), _looppool(loop)
{
	if (threadNum > 0)
	{
		_looppool.setThreadNum(threadNum);
	}
}


TCPserver::~TCPserver()
{
}
//void TCPserver::removeConnection(int fd)
//{
//	Eventloop* currloop = (loopMap.find(fd))->second;
//	currloop->assertInLoopThread();
//	currloop->removeChannel(fd);
//	//_loop->assertInLoopThread();
//	//_loop->removeChannel(fd);//怎么让对应的loop去删除channel
//	connectionMap.erase(fd);//把新建链接交给了IO线程，这个map就变成了临界值，不能这么写
//	//std::cout << "connection nums:" << connectionMap.size() << std::endl;
//	//如果存在reactor线程池，移除需要修改，connection在自己的线程调用removeconnection，
//	//应该将removechannel放在connection自己的线程中处理，TCPserver线程中仅仅删除map中的指针
//	//不再持有这个connection，通过绑定shared_from_this绑定一个删除回调函数，放在connection自身线程中执行。
//	//后期进行完善
//}
void TCPserver::removeConnection(int fd,Eventloop* currloop)
{
	currloop->assertInLoopThread();
	currloop->removeChannel(fd);
	LOG << "Disconncet with addr:" << inet_ntoa(connectionMap[currloop].find(fd)->second->getAddr().sin_addr) << "\n";
	connectionMap[currloop].erase(fd);
	//_loop->assertInLoopThread();
	//_loop->removeChannel(fd);//怎么让对应的loop去删除channel
	//connectionMap.erase(fd);//把新建链接交给了IO线程，这个map就变成了临界值，不能这么写
	//std::cout << "connection nums:" << connectionMap.size() << std::endl;
	//如果存在reactor线程池，移除需要修改，connection在自己的线程调用removeconnection，
	//应该将removechannel放在connection自己的线程中处理，TCPserver线程中仅仅删除map中的指针
	//不再持有这个connection，通过绑定shared_from_this绑定一个删除回调函数，放在connection自身线程中执行。
	//后期进行完善
}

void TCPserver::newConnnection(int fd, sockaddr_in cliaddr, Eventloop* currLoop)//把新建connection的任务丢给IO线程自己去执行
{
	ConnectionPtr _conn(new Connection(fd, currLoop, cliaddr));
	_conn->SetHandleClose(std::bind(&TCPserver::removeConnection, this, std::placeholders::_1, std::placeholders::_2));
	_conn->SetMessageCallback(_messageCallback);
	if (_writecompleteCallback)
		_conn->SetWriteCompleteCallback(_writecompleteCallback);
	_conn->activationChannel();
	connectionMap[currLoop].insert(std::make_pair(fd, _conn));
	//std::cout << "connection nums:" << connectionMap.size() << std::endl;s
}
void TCPserver::newConnectioninLoop(int fd, sockaddr_in cliaddr)
{
	Eventloop* currLoop = _looppool.getNextLoop();
	if (!currLoop->isQueueInLoop())
	{
		//考虑给fd发送一个服务器繁忙
		close(fd);
		std::cout << "add func is error" << std::endl;
	}
	currLoop->queueInLoop(std::bind(&TCPserver::newConnnection, this, fd, cliaddr, currLoop));
}
//void TCPserver::newConnnection1(int fd, sockaddr_in cliaddr, Eventloop* currLoop, ConnectionPtr _conn)//把新建connection的任务丢给IO线程自己去执行
//{
//	_conn->activationChannel();
//	connectionMap[currLoop].insert(std::make_pair(fd, _conn));
//}
//void TCPserver::newConnectioninLoop1(int fd, sockaddr_in cliaddr)
//{
//	Eventloop* currLoop = _looppool.getNextLoop();
//	ConnectionPtr _conn(new Connection(fd, currLoop, cliaddr));
//	_conn->SetHandleClose(std::bind(&TCPserver::removeConnection, this, std::placeholders::_1, std::placeholders::_2));
//	_conn->SetMessageCallback(_messageCallback);
//	if (_writecompleteCallback)
//		_conn->SetWriteCompleteCallback(_writecompleteCallback);
//	//connectionMap[currLoop].insert(std::make_pair(fd, _conn));
//	currLoop->queueInLoop(std::bind(&TCPserver::newConnnection1, this, fd, cliaddr, currLoop,_conn->shared_from_this()));
//}
void TCPserver::start()
{
	handleSIGPIPE();
	_acceptor.reset(new Acceptor(_loop));
	
	_acceptor->setnewConnCallback(std::bind(&TCPserver::newConnectioninLoop, this, std::placeholders::_1, std::placeholders::_2));
	//_acceptor->setnewConnCallback(std::bind(&TCPserver::newConnnection1, this, std::placeholders::_1, std::placeholders::_2));
	
	_looppool.start();
	//初始化connectionMap
	std::vector<Eventloop*> loops = _looppool.getAllLoops();
	if (!loops.empty())//IO池
	{
		for (Eventloop* l : loops)
		{
			std::map<int, ConnectionPtr> p;
			connectionMap.insert(std::make_pair(l, p));
		}
	}
	else//主循环
	{
		std::map<int, ConnectionPtr> p;
		connectionMap.insert(std::make_pair(_loop, p));
	}
	_acceptor->start();
}

