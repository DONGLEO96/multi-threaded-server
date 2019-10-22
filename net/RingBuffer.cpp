#include "RingBuffer.h"
#include<algorithm>
#include<string.h>
RingBuffer::RingBuffer(int _capacity)
{
	if (is_power_of_2(_capacity) != true)
	{
		_capacity = roundup_pow_of_two(_capacity);
	}
	capacity = _capacity;
	in = 0;
	out = 0;
	buffer = (Func*)malloc(capacity * sizeof(Func));
	if (buffer == NULL)
	{
		free(buffer);
		exit(1);
	}
	bufferReset();
}


RingBuffer::~RingBuffer()
{
	free(buffer);
}
bool RingBuffer::bufferIn(Func f)
{
	if (bufferIsFull())
	{
		return false;
	}
	buffer[in&(capacity-1)] = f;//队列容量是2的n次方，取余可以转换成求与
	in += 1;
	return true;
}
size_t RingBuffer::bufferOut(RingBuffer::Func* outBuffer,size_t len)
{
	len = std::min(len, bufferSize());

	for (int i = 0; i < len; ++i)
	{
		outBuffer[i] = buffer[out%capacity];
		buffer[out&(capacity-1)] = NULL;
		out += 1;
	}
	return len;
}
size_t RingBuffer::roundup_pow_of_two(size_t x)
{
	size_t b = 0;
	for (int i = 0; i < 64; ++i)
	{
		b = 1UL << i;	
		if (x <= b)
		break;
	}
	return b;
}
