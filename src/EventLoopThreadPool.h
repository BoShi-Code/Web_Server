/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:10
 *   Filename        : EventLoopThreadPool.h
 *   Description     : 
 *   创建管理线程池
 * *******************************************************/

#ifndef INCLUDE_EVENTLOOPTHREADPOOL_H
#define INCLUDE_EVENTLOOPTHREADPOOL_H

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>

#include "EventLoopThread.h"
#include "base/Logging.h"

class EventLoopThreadPool : boost::noncopyable {
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads);

    ~EventLoopThreadPool() {
        LOG << "~EventLoopThreadPool()";
    }
    void start();

    // 获取下一个 loop
    EventLoop *getNextLoop();

private:
    EventLoop* baseLoop_;

    // 线程状态
    bool started_;

    // 线程数量
    int numThreads_;

    // 下一个 thread
    int next_;

    // 线程列表
    std::vector<std::shared_ptr<EventLoopThread>> threads_;

    // 主 loop，优先级
    std::vector<EventLoop*> loops_;
}; // EventLoopThreadPool

#endif // INCLUDE_EVENTLOOPTHREADPOOL_H
