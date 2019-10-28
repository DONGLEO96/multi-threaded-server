#include "TimeStamp.h"



TimeStamp::TimeStamp():_microSecondsSinceEpoch(0)
{
}
TimeStamp::TimeStamp(int64_t us):_microSecondsSinceEpoch(us)
{
}
TimeStamp::~TimeStamp()
{
}


TimeStamp TimeStamp::Now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;
	return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
void TimeStamp::swap(TimeStamp& that)
{
	std::swap(_microSecondsSinceEpoch,that._microSecondsSinceEpoch);
}
std::string TimeStamp::toString()
{
	char buf[32] = { 0 };
	int64_t seconds = _microSecondsSinceEpoch / kMicroSecondsPerSecond;
	int64_t microseconds = _microSecondsSinceEpoch % kMicroSecondsPerSecond;
	snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}
std::string TimeStamp::toFormatteedString(bool showMicroseconds)
{
	char buf[64] = { 0 };
	time_t seconds = static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond);
	struct tm tm_time;
	gmtime_r(&seconds, &tm_time);

	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(_microSecondsSinceEpoch % kMicroSecondsPerSecond);
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
			microseconds);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	return buf;
}
void TimeStamp::addTime(double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondsPerSecond);
	_microSecondsSinceEpoch = microSecondsSinceEpoch() + delta;
}
TimeStamp TimeStamp::addTime(TimeStamp timestampo, double second)
{
	int64_t delta = static_cast<int64_t>(second * TimeStamp::kMicroSecondsPerSecond);
	return TimeStamp(timestampo.microSecondsSinceEpoch() + delta);
}
void TimeStamp::reset(TimeStamp timestamp)
{
	this->_microSecondsSinceEpoch = timestamp._microSecondsSinceEpoch;
}