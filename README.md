---
title: Multi-Threaded-Server
---
A C++ High Performance Web Server

## 1.Introduction
模仿muduo网络库实现了一个TCP服务器框架，在其上实现了一个静态http服务器（只解析GET字段），并将hexo部署在该服务器上。

测试页面：[myWeb](http://www.dongserver.cn/2019/10/25/ServerIntroduction/)

代码位置：[github](https://github.com/DONGLEO96/multi-threaded-server)

## 2.Enviroment
### OS:ubuntu 16.04
### IDE:Visual Studio2017
### Complier: g++ 5.4.0
因为对编译和Makefile文件不是很了解，在window主机上使用了Visual studio2017远程连接linux系统的功能进行编译。

## 3.Technical points
##### （1）.框架[整体模型](http://www.dongserver.cn/2019/10/25/Model/)为Reactor+线程池,主线程负责Accept新链接，并以Round Robin的方式分发给其它IO线程(兼计算线程)，如果是计算密集型服务器，也可以将IO线程与计算线程分离
##### （2）.[线程间通信方式](http://www.dongserver.cn/2019/10/24/eventfd_and_kfifo/)采用eventfd和任务队列，模仿kfifo实现了环形无锁任务队列
##### （3）.服务器框架中实现了[应用层缓冲区](http://www.dongserver.cn/2019/10/24/Buffer/)，解决了TCP通信粘包问题
##### （4）.使用双缓冲实现了高效的[异步日志](http://www.dongserver.cn/2019/10/24/Log/)
##### （5）.使用[智能指针](http://www.dongserver.cn/2019/10/24/RAII/)和[RAII](http://www.dongserver.cn/2019/10/24/RAII/)方式自动管理对象生存周期
##### （6）.实现了状态机解析HTTP（字段并不完整）
##### （7）.实现了基于小根堆的定时器

## 4.目录结构
**base:**服务器框架的中一些基础代码，主要包括对时间、互斥锁、条件变量、线程库函数的封装

**net:**服务器框架的核心代码，主要包括事件循环、连接的建立和管理

**http:**在网络框架的基础上实现的一个简单HTTP服务器，只阵地hexo博客框架解析了一些必要字段，并不完善

**main.cpp**：对http服务器的简单使用
## 5.Modle
并发模型为IO复用+非阻塞IO+线程池，其中主线程负责接收新链接并交给IO线程，IO线程负责处理连接，读取请求处理并进行回复。

如果是计算密集型任务也可以将计算任务放入专门的计算线程池，以提高IO线程响应速度。

![](/Image/ReactorModel.jpg)

## 6.Pressure measurement

测试环境：

**OS：**Ubuntu 16.04LTS

**内存：**2G

**CPU：**i3-2120

测试方法：在服务器框架上搭建了一个简单的echo服务器，使用webbench开启100个客户进程进行测试，测试结果如下：

#### 测试1
开启两个IO线程，一个接收线程，关闭日志线程，关闭所有输出功能，测试结果如下：

![](/Image/ServerWithoutLOG.jpg)

#### 测试2
开启两个IO线程，一个接收线程，开启日志线程,在连接建立和连接断开时记录日志，关闭所有输出，测试结果如下：

![](/Image/ServerWithLOG.jpg)

日志文件：

![](/Image/LOGFile.jpg)

可以看到，日志文件完成了400万行的日志记录，对服务器性能的影响几乎很小。





