#include "LogStream.h"



LogStream::LogStream()
{
}


LogStream::~LogStream()
{
}

LogStream & LogStream::operator<<(short s)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%d", s);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(unsigned short s)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%u", s);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(int i)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%ld", i);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(unsigned int i)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%lu", i);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(long i)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%ld", i);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(unsigned long i)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%lu", i);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(long long i)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%lld", i);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(unsigned long long i)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%llu", i);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(float f)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%f", f);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(double d)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%lf", d);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(long double d)
{
	if (_buffer.avail() >= kMaxNumSize)
	{
		int len = snprintf(_buffer.current(), kMaxNumSize, "%llf", d);
		_buffer.add(len);
	}
	return *this;
}

LogStream & LogStream::operator<<(char c)
{
	_buffer.append(&c, 1);
	return *this;
}

LogStream & LogStream::operator<<(const char * str)
{
	if (str)
		_buffer.append(str, strlen(str));
	else
		_buffer.append("(NULL)", 6);
	return *this;
}

LogStream & LogStream::operator<<(const string & str)
{
	_buffer.append(str.data(), str.size());
	return *this;
}
