#include "Logging.h"
#include"AsyncLogging.h"
static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static AsyncLogging* _AsyncLogging;//memory leak!!!!!!
void once_init()
{
	string pwd(getenv("PWD"));
	pwd.append("//httpServer.log");
	_AsyncLogging = new AsyncLogging(pwd);
	_AsyncLogging->start();
}
void output(const char* msg,int len)
{
	pthread_once(&once_control, once_init);
	_AsyncLogging->append(msg, len);
}
Logging::Logging(const char*filename,int line):_filename(filename),_line(line)
{
}


Logging::~Logging()
{
	getStream() << TimeStamp().Now().toFormatteedString(true) << "--" << _filename << ":line " << _line << "\n";
	const LogStream::Buffer& buf(getStream().buffer());
	output(buf.data(), buf.length());
}
