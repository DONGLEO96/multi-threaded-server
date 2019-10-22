#include "AsyncLogging.h"
#include"TimeStamp.h"


AsyncLogging::AsyncLogging(string filename, int flushInteval)
	:_flushInterval(flushInteval),
	_running(false),
	_fileName(filename),
	_thread(std::bind(&AsyncLogging::threadFunc, this)),
	_mutex(),
	_cond(_mutex),
	_currBuffer(new Buffer),
	_nextBuffer(new Buffer),
	_buffers(),
	_latch(1)
{
	_currBuffer->bzero();
	_nextBuffer->bzero();
	_buffers.reserve(16);
}


AsyncLogging::~AsyncLogging()
{
	if (_running)
		stop();
}
void AsyncLogging::append(const char* logline, int len)//写入日志，有logging类调用，在前端线程使用
{
	MutexGuard lock(_mutex);
	if (_currBuffer->avail() > len)//缓存够写直接写
		_currBuffer->append(logline, len);
	else
	{
		_buffers.push_back(_currBuffer);//当前缓冲区写满了放入后端的缓冲区列表中
		_currBuffer.reset();
		if (_nextBuffer)//如果还有nextBuffer，继续向nextbuffer里面写
			_currBuffer = std::move(_nextBuffer);
		else
			_currBuffer.reset(new Buffer);//没有nextbuffer了，创一个新的buffer
		_currBuffer->append(logline, len);
		_cond.notify();//写满了一个缓冲区需要唤醒后端线程来向文件写
	}
}

void AsyncLogging::start()
{
	_running = true;
	_thread.start();
	_latch.wait();//等到日志线程运行起来了再返回
	//预防前端线程 start(日志线程启动,但是还没有运行到func)---写入--->stop(日志线程刚好运行到func，检查_running,退出，日志丢失了)
}

void AsyncLogging::stop()
{
	_running = false;
	_cond.notify();
	_thread.join();
}
void AsyncLogging::threadFunc()//后端日志线程，将前端写入缓冲区的日志写入到文件中
{
	LogFile output(_fileName);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersTowrite;
	buffersTowrite.reserve(16);
	_latch.CountDown();
	while (_running)
	{
		MutexGuard lock(_mutex);
		if (_buffers.empty())
		{
			_cond.waitForSeconds(_flushInterval);
		}
		_buffers.push_back(_currBuffer);
		_currBuffer.reset();
		_currBuffer = std::move(newBuffer1);
		buffersTowrite.swap(_buffers);
		if (!_nextBuffer)//如果nextBuffe已经被使用过了
		{
			_nextBuffer = std::move(newBuffer2);
		}
		// 如果将要写入文件的buffer列表中buffer的个数大于25，那么将多余数据删除  
		// 消息堆积
		//前端陷入死循环，拼命发送日志消息，超过后端的处理能力
		//这是典型的生产速度超过消费速度，会造成数据在内存中的堆积
		//严重时引发性能问题(可用内存不足),
		//或程序崩溃(分配内存失败)
		if (buffersTowrite.size() > 25)
		{
			char buf[256];
			snprintf(buf, sizeof(buf), "Dropped log messages at %s, %zd larger buffers\n"
				, TimeStamp::Now().toFormatteedString(0).data(), buffersTowrite.size() - 2);
			buffersTowrite.erase(buffersTowrite.begin() + 2, buffersTowrite.end());
		}
		for (int i = 0; i < buffersTowrite.size(); ++i)
		{
			output.append(buffersTowrite[i]->data(), buffersTowrite[i]->length());
		}
		if (buffersTowrite.size() > 2)
		{
			buffersTowrite.resize(2);
		}
		if (!newBuffer1)
		{
			newBuffer1 = buffersTowrite.back();
			buffersTowrite.pop_back();
			newBuffer1->reset();
		}
		if (!newBuffer2)
		{
			newBuffer2 = buffersTowrite.back();
			buffersTowrite.pop_back();
			newBuffer2->reset();
		}
		buffersTowrite.clear();
		output.flush();
	}
	output.flush();//停止后也需要把缓冲区冲洗一次；
}