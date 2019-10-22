#pragma once
#include<vector>
#include<unistd.h>

/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size

class Buffer
{
public:
	Buffer();
	~Buffer();
	size_t readableBytes() { return writeIndex - readIndex; }
	size_t writeableBytes() { return _buffer.capacity() - writeIndex; }
	size_t prependBytes() { return readIndex; }
	char* begin() { return &*_buffer.begin(); }
	char* beginRead() { return begin() + readIndex; }
	char* beginWrite() { return begin() + writeIndex; }
	int findcrlf();
	void append(char* data,size_t len);
	void ensureWriteableBytes(size_t len);
	void makeSpace(size_t len);
	ssize_t readFd(int fd, int* errorNum);
	void retrive(size_t len);
	void retriveAll();
	char& operator[](int i);
private:
	const size_t _initSize;
	size_t readIndex;
	size_t writeIndex;
	std::vector<char> _buffer;
};

