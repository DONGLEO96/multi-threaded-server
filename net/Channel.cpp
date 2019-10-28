#include "Channel.h"
#include "TCPserver.h"
#include"Eventloop.h"
#include<iostream>
#include<sys/poll.h>
const int Channel::ReadEvent = POLLIN|POLLPRI;
const int Channel::NoneEvent = 0;
const int Channel::WriteEvent = POLLOUT;
Channel::Channel(int sockfd, Eventloop* loop) :sock_fd(sockfd), _event(0), _revent(0), _index(-1),
												readCallback(NULL),writeCallback(NULL),closeCallback(NULL),_loop(loop),
												isSetTie(false)
{

}
Channel::~Channel()
{
	close(sock_fd);
	//std::cout << "channel destructor" << std::endl;
}
void Channel::EnableReading()
{
	_event |= ReadEvent;
	update();
}
void Channel::disableReading()
{
	_event &= ~ReadEvent;
	update();
}
void Channel::EnableWriting()
{
	_event |= WriteEvent;
	update();
}
void Channel::disableWriting()
{
	_event &= ~WriteEvent;
	update();
}
void Channel::disableAll()
{
	_event = NoneEvent;
	update();
}
void Channel::update()
{
	_loop->update(this);
}
void Channel::handleEvent()
{
	std::shared_ptr<Connection> guard = _tie.lock();
	if (isSetTie==false|| guard != NULL)
	{
		{
			if (_revent&POLLIN)
				if (readCallback)
					readCallback();
				else
				{
					printf("readCallback is NULL\n");
				}
			if (_revent& POLLOUT)
				if (writeCallback)
					writeCallback();
				else
				{
					printf("writeCallback is NULL\n");
				}
			if ((_revent&POLLHUP) && !(_revent&POLLIN))
				if (closeCallback)
					closeCallback();
				else
				{
					printf("closeCallback is NULL\n");
				}
		}
	}
}