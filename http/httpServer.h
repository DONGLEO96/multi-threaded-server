#pragma once
#include"TCPserver.h"
#include"Connection.h"
#include"Eventloop.h"
#include<memory>
#include<map>
#include<unordered_set>
#include<queue>
//长连接还不是很懂，先不写
struct Entry;
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
typedef std::weak_ptr<Connection> WeakConnPtr;
typedef std::weak_ptr<Entry> WeakEntryPtr;
typedef std::shared_ptr<Entry> EntryPtr;
typedef std::map<Eventloop*, std::unordered_set<EntryPtr>> Bucket;//桶里面放Entry，定时器触发一次加一个新桶丢一个旧桶
typedef std::queue<Bucket> WeakConnList;
struct Entry//用于包裹Connection,Entry析构时断开Conntion连接
{
	explicit Entry(const WeakConnPtr& weakConn):_weakconn(weakConn)
	{
	}
	~Entry()
	{
		TCPserver::ConnectionPtr conn = _weakconn.lock();//如果连接还在就关闭连接
		delete connContext;
		if (conn)
		{
			conn->setContext(NULL);//防止conn再使用这个值
			conn->handleClose(conn->getSockfd());
		}
		
	}
	void setconnContext(WeakEntryPtr* _context)
	{
		connContext = _context;//析构的时候负责把这个值delete掉，Connection类处理 不掉
	}
	WeakConnPtr _weakconn;
	WeakEntryPtr* connContext;
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
	void connCallBack(TCPserver::ConnectionPtr conn);
	void onTimer();
	std::string sourceFilePath;
	int sourceDirfd;
	WeakConnList connBuckets;





	//char* url;
	//char* version;
	//bool keep_alive;//暂时不用
	//int content_length;
	//char* host;

};

