#pragma once
#include<string>
#include<string.h>
using namespace std;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;
template<int SIZE>
class FixedBuffer
{
public:
	FixedBuffer()
		: _cur(data_)
	{ }

	~FixedBuffer()
	{ }

	void append(const char* buf, size_t len)
	{
		if (avail() > static_cast<int>(len))
		{
			memcpy(_cur, buf, len);
			_cur += len;
		}
	}

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(_cur - data_); }

	char* current() { return _cur; }
	int avail() const { return static_cast<int>(end() - _cur); }
	void add(size_t len) { _cur += len; }

	void reset() { _cur = data_; }
	void bzero() { memset(data_, 0, sizeof data_); }


private:
	const char* end() const { return data_ + sizeof data_; }

	char data_[SIZE];
	char* _cur;//µ±«∞÷∏’Î
};
class LogStream
{
public:
	typedef FixedBuffer<kSmallBuffer> Buffer;
	LogStream();
	~LogStream();
	LogStream& operator<<(bool v)
	{
		_buffer.append(v ? "1" : "0", 1);
		return *this;
	}
	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(long double);
	LogStream& operator<<(char);
	LogStream& operator<<(const char* str);
	LogStream& operator<<(const string&);
	const Buffer& buffer(){ return _buffer; }
	void resetBuffer() { _buffer.reset(); }
private:
	Buffer _buffer;
	const int kMaxNumSize = 32;

};

