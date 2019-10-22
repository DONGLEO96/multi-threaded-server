#include "Buffer.h"
#include<sys/uio.h>
#include<errno.h>
Buffer::Buffer():_initSize(1024),readIndex(0),writeIndex(0),_buffer(_initSize)
{
}


Buffer::~Buffer()
{
}
int Buffer::findcrlf()
{
	for (auto it = beginRead(); it != beginWrite(); ++it)
	{
		if (*it == '\r')
		{
			if (it + 1 == beginWrite())
			{
				return -1;
			}
			else
			{
				if (*(it + 1) == '\n')
				{
					return (it+1-beginRead()+1);//返回可读的字节数  a b \r \n
				}
			}
		}
	}
	return -1;
}
void Buffer::append(char* data, size_t len)

{
	ensureWriteableBytes(len);
	std::copy(data, data + len, beginWrite());
	writeIndex += len;
}
void Buffer::ensureWriteableBytes(size_t len)
{
	if (len > writeableBytes())
		makeSpace(len);
}
void Buffer::makeSpace(size_t len)
{
	if (prependBytes() + writeableBytes() < len)
	{
		_buffer.resize(writeIndex + len);
	}
	else
	{
		size_t readable = readableBytes();
		std::copy(begin() + readIndex, begin() + writeIndex, begin());
		readIndex = 0;
		writeIndex = readable;
	}
}
void Buffer::retrive(size_t len)
{
	if (len < readableBytes())
		readIndex += len;
	else
		retriveAll();
}
void Buffer::retriveAll()
{
	readIndex = 0;
	writeIndex = 0;
}
ssize_t Buffer::readFd(int fd, int* errorNum)
{
	char extraBuf[65536];
	struct iovec vec[2];
	const size_t writeable = writeableBytes();
	vec[0].iov_base = begin() + writeIndex;
	vec[0].iov_len = writeable;
	vec[1].iov_base = extraBuf;
	vec[1].iov_len = sizeof(extraBuf);
	ssize_t n = readv(fd, vec,2);
	if (n < 0)
	{
		*errorNum = errno;
	}
	else if (static_cast<size_t> (n) <= writeable)
	{
		writeIndex += n;
	}
	else
	{
		writeIndex = _buffer.size();
		append(extraBuf, n - writeable);
	}
	return n;
}
char& Buffer::operator[](int i)
{
	return *(this->beginRead()+i);
}