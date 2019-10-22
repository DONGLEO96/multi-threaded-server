#include "httpServer.h"
#include<string>
#include<string.h>
#include<iostream>
#include"util.h"
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
httpServer::httpServer(int threadNum) :loop(), tcpserver(&loop, threadNum), sourceFilePath()
{
	//std::cout << "httpServer constructor" << std::endl;
}


httpServer::~httpServer()
{
}
void httpServer::start()
{
	//std::cout << "httpServer start" << std::endl;
	if (sourceFilePath.size() == 0)
	{

		sourceFilePath = std::string(getenv("PWD"));
		sourceFilePath.append("//source");
	}
	sourceDirfd = open(sourceFilePath.data(), O_RDONLY);
	tcpserver.setMessageCallback(std::bind(&httpServer::handleRead, this, std::placeholders::_1, std::placeholders::_2));
	tcpserver.start();
	loop.loop();
	//tcpserver.setMessageCallback(std::bind(&httpServer::parse_http, this, std::placeholders::_1, std::placeholders::_2));
	//tcpserver.start();
	//loop.loop();
}
void httpServer::setSourceFile(std::string  path)
{
	sourceFilePath = path;
}
LINE_STATUS httpServer::parse_line(Buffer & inBuffer,int& index)//判断是否收到一行完整数据
{
	index = inBuffer.findcrlf();
	if (index == -1)
	{
		return LINE_CONTINUE;
	}
	else
	{
		return LINE_OK;
	}
}

HTTP_CODE httpServer::parse_requestline(Buffer& inbuffer,int index, requestHeadData& data,CHECK_STATE& checkstate)//解析请求行
{
	if (*(inbuffer.beginRead()) == '\r'&&*(inbuffer.beginRead() + 1) == '\n')
	{
		inbuffer.retrive(2);
		return NO_REQUEST;
		
	}
	std::vector<char*> request = spilt(inbuffer.beginRead(),index, ' ');
	if (request.size() != 3)
	{
		return BAD_REQUEST;
	}
	data.url = std::string(request[1]);
	//data.version = request[2];
	if (strcasestr(request[0],"GET")!=NULL)
	{
		data.method = GET;
	}
	else
	{
		//inbuffer.retrive(index);badrequest会直接关闭连接，析构connection，不用管buffer中的内容
		return BAD_REQUEST;
	}
	if (strcasestr(request[2], "1.1") != NULL)
	{
		data.version = HTTP_11;
	}
	else if (strcasestr(request[2], "1.0") != NULL)
	{
		data.version = HTTP_10;
	}
	else
	{
		//inbuffer.retrive(index);
		return BAD_REQUEST;
	}
	inbuffer.retrive(index);
	checkstate = CHECK_STATE_HEADER;
	return NO_REQUEST;
}

HTTP_CODE httpServer::parse_header(Buffer& inbuffer,int index,requestHeadData& data,CHECK_STATE& checkstate)//解析头文件
{
	char* temp = inbuffer.beginRead();
	if (*temp =='\r'&& *(temp+1) == '\n')
	{
		if(data.contentLength!=0)
		{
			checkstate = CHECK_STATE_CONTENT;
			return NO_REQUEST;
		}
		return GET_REQUEST;
	}
	*(temp + index - 1) = '\0';
	if (strncasecmp(inbuffer.beginRead(), "Connection:", 11) == 0)
	{
		if (strcasestr(inbuffer.beginRead(),"close")==NULL)
		{
			data.keep_alive = true;
		}
	}
	else if (strncasecmp(inbuffer.beginRead(), "Content-Length:", 15) == 0)
	{
		char* t = strcasestr(inbuffer.beginRead(), " ");
		if (t == NULL)
			return BAD_REQUEST;
		else
		{
			data.contentLength = atoi(t + 1);
		}

	}
	//else if (strncasecmp(header.data(), "Host", 5) == 0)
	//{
	//	int index = header.find(" ");
	//	if (index == std::string::npos)
	//		return BAD_REQUEST;
	//	host = const_cast<char*>(header.data() + index + 1);
	//}
	else
	{
		//printf("unknow header %s/n", header.data());
	}
	inbuffer.retrive(index);
	return NO_REQUEST;
}

HTTP_CODE httpServer::parse_content(Buffer & inBuffer,int& content_length,int lineSize)//解析消息体
{
	if (inBuffer.readableBytes() >= content_length)
	{
		inBuffer.retrive(content_length);
		return GET_REQUEST;
	}
	else if (lineSize != 0)
	{
		content_length -= lineSize;
	}
	return NO_REQUEST;
}

HTTP_CODE httpServer::parse_http(Buffer& inBuffer,requestHeadData& data)
{
	LINE_STATUS line_status = LINE_OK;
	HTTP_CODE ret = NO_REQUEST;
	CHECK_STATE checkstate= CHECK_STATE_REQUESTLINE;
	//string version;
	//int content_length=0;
	//char* host;
	//METHOD method;
	int index;
	while (((checkstate == CHECK_STATE_CONTENT) && (line_status = LINE_OK)) || ((line_status = parse_line(inBuffer, index) )== LINE_OK))
	{
		
		//std::cout << "got 1 line:" << line << std::endl;
		switch (checkstate)
		{
			case CHECK_STATE_REQUESTLINE:
			{
				ret = parse_requestline(inBuffer, index, data, checkstate);
				if (ret == BAD_REQUEST)
				{
					//std::cout << "BAD_REQUEST" << std::endl;
					return BAD_REQUEST;
				}
				break;
			}
			case CHECK_STATE_HEADER:
			{
				ret = parse_header(inBuffer,index, data, checkstate);
				if (ret == BAD_REQUEST)
				{
					//std::cout << "BAD_REQUEST" << std::endl; 
					return BAD_REQUEST;
				}
				else if (ret == GET_REQUEST)
				{
					//std::cout << "received an request,response the request" << std::endl;
					return GET_REQUEST;
				}
				index = -1;
				break;
			}
			case CHECK_STATE_CONTENT:
			{
				ret = parse_content(inBuffer,data.contentLength,index);
				if (ret == GET_REQUEST)
				{
					//std::cout << "received an request,response the request" << std::endl;
					return GET_REQUEST;
				}
				//line_status = LINE_CONTINUE;//这是个忙等
				break;
			}
			default:
				//std::cout << "INTERNAL_ERROR" << std::endl;
				return INTERNAL_ERROR;
		}
	}
}

HTTP_CODE httpServer::get_file(std::string & url,struct stat& file_stat,int& fd)
{
	if (url[url.size() - 1] == '/')
	{
		url.append("index.html");
	}
	if (url.find("favicon")!=std::string::npos)
	{
		return NO_SOURCE;
	}
	if (fstatat(sourceDirfd,url.data()+1, &file_stat,0)<0)
	{
		return NO_SOURCE;
	}
	if (S_ISDIR(file_stat.st_mode))//请求的是一个文件夹，默认访问这个文件夹的中的index.html
	{
		url.append("/index.html");
		if(fstatat(sourceDirfd, url.data() + 1, &file_stat, 0) < 0)//如果没有，返回错误
			return BAD_REQUEST;
	}
	fd = openat(sourceDirfd, url.data() + 1, O_RDONLY);
	return GET_REQUEST;
}

void httpServer::handleRead(TCPserver::ConnectionPtr connPtr, Buffer & inBuffer)
{
	requestHeadData requestData;
	struct stat file_stat; 
	int fd;
	switch (parse_http(inBuffer,requestData))
	{
	case(GET_REQUEST):
	{
		switch (get_file(requestData.url,file_stat,fd))
		{
		case(GET_REQUEST):
		{
			char* file_addr = (char*)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);//用mmap函数省去一次读入
			char* data = new char[256 + file_stat.st_size];
			addStatusLine(200, "OK", data);//状态行
			parse_contentType(requestData);
			addHeader(requestData.keep_alive, file_stat.st_size, data,sizeof(data),requestData.type.data());//头部
			int headerlen = strlen(data);
			memcpy(data + headerlen, file_addr, file_stat.st_size);//拼接正文
			//snprintf(data + strlen(data), sizeof(data) - strlen(data), "%s", file_addr);
			//std::cout << data << std::endl;
			connPtr->send(data, headerlen + file_stat.st_size);//图片文件中有'\0'，不能用strlen计算长度
			if(requestData.keep_alive==false)
				connPtr->handleClose(connPtr->getSockfd());
			delete data;
			break;
		}
		case(BAD_REQUEST):		
		{
			char data[256];
			addStatusLine(400, "Bad Request", data);
			string message("<html>Bad Request</html>");
			addHeader(requestData.keep_alive, message.size(), data, sizeof(data), 0);
			snprintf(data + strlen(data), sizeof(data) - strlen(data), "%s", message.data());
			connPtr->send(data, strlen(data));
			if (requestData.keep_alive == false)
				connPtr->handleClose(connPtr->getSockfd());
			break;
		}
		case(NO_SOURCE):
		{
			char data[256];
			addStatusLine(400, "NO Source", data);
			string message("<html>NO Source</html>");
			addHeader(requestData.keep_alive, message.size(), data, sizeof(data),0);
			snprintf(data + strlen(data), sizeof(data) - strlen(data), "%s", message.data());
			connPtr->send(data, strlen(data));
			if (requestData.keep_alive == false)
				connPtr->handleClose(connPtr->getSockfd());
			break;
		}
			
		}
		//if (requestData.url.find("index") != std::string::npos)//测试并发量使用，避免文件操作拖慢服务器速度。
		//{
		//	char data[256];
		//	string message("<html>Hello World!</html>");//当作index.html,以后有机会再添加文件读取功能
		//	addStatusLine(200, "OK", data);
		//	addHeader(requestData.keep_alive, message.size(), data,sizeof(data));
		//	snprintf(data + strlen(data), sizeof(data) - strlen(data), "%s", message.data());
		//	//std::cout << data << std::endl;
		//	connPtr->send(data, strlen(data));
		//	if(requestData.keep_alive==false)
		//		connPtr->handleClose(connPtr->getSockfd());
		//}
		////connPtr->send("");
		//else if (requestData.url.find("favicon.ico") != std::string::npos)
		//{
		//	char data[1024];
		//	addStatusLine(200, "OK", data);
		//	addHeader_ico(data, sizeof(data));
		//	snprintf(data + strlen(data), sizeof(data) - strlen(data), "%s", favicon);
		//	connPtr->send(data, strlen(data));
		//	if (requestData.keep_alive == false)
		//		connPtr->handleClose(connPtr->getSockfd());
		//}
		break;
	}
	case(BAD_REQUEST):
	{
		char data[256];
		addStatusLine(400, "Bad Request", data);
		string message("<html>Bad Request</html>");
		addHeader(requestData.keep_alive, message.size(), data, sizeof(data), 0);
		snprintf(data + strlen(data), sizeof(data) - strlen(data), "%s", message.data());
		connPtr->send(data, strlen(data));
		if (requestData.keep_alive == false)
			connPtr->handleClose(connPtr->getSockfd());
	}
	default:
	{
		//std::cout << "error request" << std::endl;
		break;
	}
	}
}

void httpServer::addStatusLine(int status, const char * title,char* data)
{
	snprintf(data, 256, "%s %d %s\r\n", "HTTP/1.1", status, title);
}

void httpServer::addHeader(bool keep_alive,int content_len, char* data,int dataSize,const char* content_type)
{
	snprintf(data + strlen(data), dataSize - strlen(data), "Connectin:%s\r\nContent-Type:%s\r\nContent-Length:%d\r\n\r\n",
		(keep_alive == true) ? "keep-alive" : "close", (content_type == 0) ? "text/html" : content_type, content_len);
}
void httpServer::addHeader_ico(char* data, int dataSize)
{
	snprintf(data + strlen(data), dataSize - strlen(data), "Content-Type:image/png\r\n\r\n");
}
void httpServer::parse_contentType(requestHeadData& data)
{
	int index = data.url.rfind(".");
	std::string typestring(data.url.data() + index + 1);
	if (typestring == "html")
	{
		data.type = "text/html";
	}
	else if (typestring == "css")
	{
		data.type = "text/css";
	}
	else if (typestring == "js")
	{
		data.type = "application/x-javascript";
	}
	else if (typestring == "jpg")
	{
		data.type = "image/jpeg";
	}
	else if (typestring == "ico")
	{
		data.type = "image/x-icon";
	}
	else
	{
		data.type == "text/html";
	}
}
