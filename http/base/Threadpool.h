#pragma once
#include<string>
#include"Thread.h"
#include<memory>
#include<queue>
class Threadpool
{
public:
	typedef std::function<void()> Task;
	Threadpool(std::string name = std::string("threadpool"));
	~Threadpool();
	void start(int);
	void stop();
	void addTask(Task task);
	size_t getTaskNum() { return _taskqueue.size(); }
	std::string& getThreadPoolName() { return _name; }
	bool isStarted() { return _started; }
private:
	Mutex _mutex;
	Condition notEmpty;
	bool _started;
	std::string _name;
	//TaskQueue<Task> _taskqueue;//不使用这个类来实现任务队列，因为当任务队列没有任务时会让任务线程一直阻塞，失去对其的控制
	std::queue<Task> _taskqueue;
	int _threadNums;
	std::vector< std::unique_ptr<Thread> > _threads;
	Task take();
	void doTask();
};
