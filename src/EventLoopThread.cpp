/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:24
 *   Filename        : EventLoopThread.cpp
 *   Description     : IO 线程，执行特定任务
 * *******************************************************/

#include <functional>

#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
    :loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex_(),
    cond_(mutex_)
{}

EventLoopThread::~EventLoopThread() {
    // 线程结束时清理
    exiting_ = true;
    if (loop_ != NULL) {

        // 终止 IO 线程循环
        loop_->quit();

        // 清理 IO 线程，线程等待
        // 规避内存泄漏
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        // std::lock_guard<std::mutex> lock(mutex_);
        MutexLockGuard lock(mutex_);
        
        // 一直等到threadFun在Thread里真正跑起来
        while (loop_ == NULL)
            cond_.wait();
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    {
        // std::lock_guard<std::mutex> lock(mutex_);
        MutexLockGuard lock(mutex_);
        loop_ = &loop;

        // 条件变量 cond 唤醒线程
        cond_.notify();
    }

    loop.loop();
    //assert(exiting_);
    loop_ = NULL;
}
