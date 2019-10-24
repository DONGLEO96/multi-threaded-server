#include "FileUtil.h"



FileUtil::FileUtil(string& filename):_file(fopen(filename.data(),"ab"))
{
	setbuf(_file, buffer);
}


FileUtil::~FileUtil()
{
	fclose(_file);
}
void FileUtil::append(const char* logline, const size_t len)
{
	size_t n = write(logline, len);
	size_t remain = len - n;
	while (remain > 0)
	{
		size_t x = write(logline + n, remain);
		if (x == 0)
		{
			int err = ferror(_file);
			if(err)
				fprintf(stderr, "FileUtil::append() failed !\n");
			break;
		}
		n += x;
		remain = len - n;
	}
}
void FileUtil::flush()
{
	fflush(_file);
}
size_t FileUtil::write(const char* logline, size_t len)
{
	return fwrite_unlocked(logline, 1, len, _file);
}
