#pragma once
#include<stdint.h>
#include<string>
#include<sys/time.h>
#include<inttypes.h>
class TimeStamp
{
public:
	TimeStamp();
	explicit TimeStamp(int64_t ms);
	~TimeStamp();
	void swap(TimeStamp& that);
	std::string toString();
	std::string toFormatteedString(bool showMicroseconds);
	int64_t microSecondsSinceEpoch() { return this->_microSecondsSinceEpoch; }
	time_t SecondSinceEpoch() { return static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond); }
	bool Valid() { return this->_microSecondsSinceEpoch > 0; }
	void addTime(double seconds);
	void reset(TimeStamp timestamp);
	static TimeStamp addTime(TimeStamp timestamp, double seconds);
	static TimeStamp Now();
	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t _microSecondsSinceEpoch;
};
inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}
inline bool operator<=(TimeStamp lhs, TimeStamp rhs)
{
	return lhs.microSecondsSinceEpoch() <= rhs.microSecondsSinceEpoch();
}
inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}
inline bool operator>(TimeStamp lhs, TimeStamp rhs)
{
	return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
}
inline bool operator>=(TimeStamp lhs, TimeStamp rhs)
{
	return lhs.microSecondsSinceEpoch() >= rhs.microSecondsSinceEpoch();
}

