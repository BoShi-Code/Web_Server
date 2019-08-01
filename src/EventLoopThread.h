/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:09
 *   Filename        : EventLoopThread.h
 *   Description     : IO 线程类，执行特定任务，如：日志打印
 *   线程池是工作线程
 * *******************************************************/

#ifndef INCLUDE_EVENTLOOPTHREAD_H
#define INCLUDE_EVENTLOOPTHREAD_H

#include <boost/noncopyable.hpp>

#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "EventLoop.h"

class EventLoopThread : boost::noncopyable {
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

    // 线程运行的 loop 循环
    EventLoop *loop_;
    bool exiting_;

    // 线程对象
    // std::thread thread_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
}; // EventLoop

#endif // INCLUDE_EVENTLOOPTHREAD_H
