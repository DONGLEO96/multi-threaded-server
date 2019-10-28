#pragma once
#include"TCPserver.h"
#include"Connection.h"
#include"Eventloop.h"
#include<memory>
#include<map>
//长连接还不是很懂，先不写
enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER ,CHECK_STATE_CONTENT};//主状态机，状态分别是解析请求行和解析头文件
enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_CONTINUE };//从状态机，用去分析是否得到一行数据
enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST,INTERNAL_ERROR,NO_SOURCE };//分析请求行的状态
enum METHOD { GET = 0, HEAD, UNREALIZED };
enum VERSION { HTTP_11 = 0, HTTP_10 };
enum FILETYPE {HTML=0,JPG,JS,CSS};
static std::map<std::string, std::string> contenttypeMap;
struct requestHeadData//只解析这几个字段
{
	VERSION version;
	std::string url;
	METHOD method=UNREALIZED;
	bool keep_alive = false;
	int contentLength=0;
	std::string type;
};
class httpServer
{
public:
	httpServer(int threadNum);
	~httpServer();
	void start();
	void setSourceFile(std::string path);
private:
	//std::unique_ptr<TCPserver> tcpserver;
	Eventloop loop;
	TCPserver tcpserver;
	LINE_STATUS parse_line(Buffer& inBuffer, int& index);
	HTTP_CODE parse_requestline(Buffer& inbuffer, int index, requestHeadData& data, CHECK_STATE& checkstate);
	HTTP_CODE parse_header(Buffer& inbuffer, int index, requestHeadData& data, CHECK_STATE& checkstate);
	HTTP_CODE parse_content(Buffer & inBuffer, int& content_length, int lineSize);
	HTTP_CODE parse_http(Buffer& inBuffer, requestHeadData& data);
	HTTP_CODE get_file(std::string& url, struct stat& file_stat, int& fd);
	void parse_contentType(requestHeadData& data);
	//HTTP_CODE parse_url(string& url);//暂时不写
	void handleRead(TCPserver::ConnectionPtr connPtr, Buffer& inBuffer);
	void addStatusLine(int status, const char* title,  char* data);
	void addHeader(bool keep_alive,int content_len,  char* data,int dataSize,const                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                char* content_type);
	void addHeader_ico(char * data, int dataSize);
	std::string sourceFilePath;
	int sourceDirfd;





	//char* url;
	//char* version;
	//bool keep_alive;//暂时不用
	//int content_length;
	//char* host;

};

