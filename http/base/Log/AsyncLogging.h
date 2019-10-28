#pragma once
#include"LogFile.h"
#include"LogStream.h"
#include<vector>
#include"Thread.h"
using namespace std;
class AsyncLogging
{
public:
	AsyncLogging(const string filename, int flushInterval = 2);
	~AsyncLogging();
	void append(const char* logline, int len);
	void start();
	void stop();
private:
	void threadFunc();
	typedef FixedBuffer<kLargeBuffer> Buffer;
	typedef vector<shared_ptr<Buffer>> BufferVector;
	typedef shared_ptr<Buffer> BufferPtr;
	const int _flushInterval;
	bool _running;
	string _fileName;
	Thread _thread;
	Mutex _mutex;
	Condition _cond;
	BufferPtr _currBuffer;
	BufferPtr _nextBuffer;
	BufferVector _buffers;
	CountLatch _latch;
};

