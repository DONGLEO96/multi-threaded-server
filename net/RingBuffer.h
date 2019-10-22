#pragma once
#include<functional>
class RingBuffer
{
public:
	typedef std::function<void()> Func;
	RingBuffer(int _capacity);
	~RingBuffer();
	inline void bufferReset() { in = out = 0; }
	inline size_t bufferCapacity() { return capacity; }//all space
	inline size_t bufferSize() { return in - out; }//used space
	inline size_t bufferAvail() { int i = bufferCapacity() - bufferSize(); return bufferCapacity() - bufferSize(); }
	inline bool buffeIsEmpty() { return bufferSize() == 0; }
	inline bool bufferIsFull() { return bufferAvail() == 0; }
	bool bufferIn(Func f);
	size_t bufferOut(Func* out,size_t len);
private:
	Func* buffer;
	size_t capacity;
	size_t in;
	size_t out;
	inline bool is_power_of_2(size_t x) { return (x != 0) & (x&(x - 1)) == 0; }
	size_t roundup_pow_of_two(size_t x);
};

