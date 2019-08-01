/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:08
 *   Filename        : EventLoop.h
 *   Description     : IO 复用流程的数据抽象
 *   等待事件、处理事件、执行其他业务
 * *******************************************************/

#ifndef INCLUDE_EVENTLOOP_H
#define INCLUDE_EVENTLOOP_H

#include <vector>
#include <memory>
#include <functional>
#include <iostream>

#include "base/Thread.h"
#include "Epoll.h"
#include "base/Logging.h"
#include "Channel.h"
#include "base/CurrentThread.h"
#include "Util.h"

class EventLoop {
public:
    // 回调函数类型，及业务类型
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    bool isInLoopThread() const {
        return threadId_ == CurrentThread::tid();
    }
    void assertInLoopThread() {
        assert(isInLoopThread());
    }

    // 关闭文件描述符
    void shutdown(std::shared_ptr<Channel> channel) {
        shutDownWR(channel->getFd());
    }

    // 移除事件
    void removeFromPoller(std::shared_ptr<Channel> channel) {
        //shutDownWR(channel->getFd());
        poller_->epoll_del(channel);
    }

    // 修改事件
    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0) {
        poller_->epoll_mod(channel, timeout);
    }

    // 添加事件
    void addToPoller(std::shared_ptr<Channel> channel, int timeout = 0) {
        poller_->epoll_add(channel, timeout);
    }
    
private:
    // 声明顺序 wakeupFd_ > pwakeupChannel_
    bool looping_;

    // epoll 对象
    std::shared_ptr<Epoll> poller_;

    // 跨线程唤醒 fd
    int wakeupFd_;

    // 线程运行状态
    bool quit_;
    bool eventHandling_;

    // 保护任务列表的互斥量
    mutable MutexLock mutex_;

    // 任务列表
    std::vector<Functor> pendingFunctors_;
    bool callingPendingFunctors_;

    // loop 所在线程 id
    const pid_t threadId_; 

    // 事件列表类型
    std::shared_ptr<Channel> pwakeupChannel_;
    
    // 唤醒 loop
    void wakeup();

    // 唤醒 loop 的读事件回调
    void handleRead();
    void doPendingFunctors();

    // 唤醒 loop 后的错误事件回调
    void handleConn();
}; // EventLoop

#endif // INCLUDE_EVENTLOOP_H
