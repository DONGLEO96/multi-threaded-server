#pragma once
#include"LogStream.h"
#include"pthread.h"
#include"string"
#include"stdio.h"
#include"TimeStamp.h"
using namespace std;
class Logging
{
public:
	Logging(const char* fileName,int line);
	~Logging();
	LogStream& getStream() { return _stream; }
	static string getlogFileName()
	{
		return logFileName;
	}
private:
	int _line;
	string _filename;
	static string logFileName;
	LogStream _stream;
};
#define LOG Logging(__FILE__,__LINE__).getStream()
