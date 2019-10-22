#include "Connection.h"
#include<unistd.h>
#include<iostream>
#include"Eventloop.h"
#include"Logging.h"
Connection::Connection(int sockfd, Eventloop* loop, struct sockaddr_in cliaddr) :fd(sockfd),_loop(loop),
														channel(new Channel(sockfd, _loop)), inBuffer(),
														outBuffer(),_cliaddr(cliaddr)
{
	//std::cout << "connection constructor" << std::endl;
	//channel->EnableReading();不能在这儿更新监听通道，多线程会出错
	if (_connectionCallback)
		_connectionCallback(shared_from_this());
	else
		defaultConnectionCallback();
	channel->setReadCallback(std::bind(&Connection::handleRead, this, fd));
	channel->setWriteCallback(std::bind(&Connection::handleWrite, this, fd));
	channel->setCloseCallback(std::bind(&Connection::handleClose, this, fd));
	///_loop->wakeup();//给IO线程发送任务时IO挂起在poll的话会检测不到新的sockfd放入了被检测队列，对他进行唤醒一次
}
Connection::~Connection()
{
	//std::cout << "connection destructor" << std::endl;
}
void Connection::handleClose(int fd)
{
	_loop->runInLoop(bind(_closeCallback, fd, _loop));
}
void Connection::SetHandleClose(CloseCallback cb)
{
	_closeCallback = cb;
}
void Connection::handleRead(int fd)
{
	
	int errorNum;
	ssize_t nums = inBuffer.readFd(fd, &errorNum);
	//ssize_t nums = read(fd, Buffer, 1024);
	if (nums < 0)
	{
		//std::cout << "inputBuffer read error" << std::endl << errorNum << std::endl;.

		LOG << "error:"<< inet_ntoa(this->_cliaddr.sin_addr) << " send " << nums << " bytes" << "\n";
	}
	if (nums == 0)
	{
		handleClose(fd);
	}
	if (nums > 0)
	{
		LOG << inet_ntoa(this->_cliaddr.sin_addr) << " send " << nums << " bytes" << "\n";
		//write(fd, Buffer, nums);//应以messageCallback代替，将控制权交给用户，由用户传入TCPserver，再传入Connection
		_messageCallback(shared_from_this(),inBuffer);//回调messageCallback，后期加入线程池，将messageCallback中的任务放入线程池中处理
	}

}
void Connection::send(char* data,size_t len)
{
	if (_loop->isLoopThread())
		sendinLoop(data,len);
	else
	{
		_loop->queueInLoop(std::bind(&Connection::sendinLoop, this, data,len));
	}
}
void Connection::send(std::string  data)
{
	//send(const_cast<char*>(data.data()), data.size());
}
void Connection::activationChannel()
{
	channel->tie(shared_from_this());
	channel->EnableReading();
}
void Connection::sendinLoop(char* data,size_t len)
{
	ssize_t n = 0;
	if (!channel->isWriting() && outBuffer.readableBytes() == 0)
	{
		n = write(fd, data, len);
		if (n < 0)
		{
			LOG << "error:" << " send " << n << " bytes to" << inet_ntoa(this->_cliaddr.sin_addr) << "\n";
			//std::cout << "send error" << std::endl << errno << std::endl;
		}
		if (static_cast<size_t>(n) == len)//最多一次能发两百多万byte
		{
			if (_writecompleteCallback)
			{
				_loop->queueInLoop(std::bind(_writecompleteCallback, shared_from_this()));
			}
			else
			{
				//std::cout << "send complete" << std::endl;
			}
			LOG << " send " << n << " bytes to" << inet_ntoa(this->_cliaddr.sin_addr) << "\n";
		}	
	}
	if (static_cast<size_t>(n) < len)
	{
		outBuffer.append(data + n, len - n);
		channel->EnableWriting();
	}
}
void Connection::handleWrite(int fd)
{
	if (channel->isWriting())
	{
		ssize_t n = write(fd,outBuffer.beginRead(),outBuffer.readableBytes());
		//std::cout << "send:" << n << "Bytes" << std::endl;
		if (n > 0)
		{
			outBuffer.retrive(n);
			if (outBuffer.readableBytes() == 0)
			{
				channel->disableWriting();
				//有BUG,可能在对端已经关闭，poll检查到，调用read，read到0，调用close，close会在事件循环结束时调用
				//此时本地socket fd并未关闭，不过往里面写数据会引起SIGPIPE。
				//还没想到办法解决，先放着吧
				if (_writecompleteCallback)
				{
					_loop->queueInLoop(std::bind(_writecompleteCallback, shared_from_this()));//放在时间循环后执行，以免阻塞其他线程事件
				}
				else
				{
					//std::cout << "handlewrite complete" << std::endl;
				}
			}
		}
	}
	else
	{
		//std::cout << "no more write" << std::endl;
	}
}
void Connection::defaultConnectionCallback()
{
	//std::cout << "connfd:" << fd << std::endl << "client addr:" << inet_ntoa(_cliaddr.sin_addr)
		//<< std::endl << "client port:" << ntohs(_cliaddr.sin_port) << std::endl;
}



