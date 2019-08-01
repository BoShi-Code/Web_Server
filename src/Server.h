/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:14
 *   Filename        : Server.h
 *   Description     : 
 * *******************************************************/

#ifndef INCLUDE_SERVER_H
#define INCLUDE_SERVER_H

#include <memory>

#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"

class Server {
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server() {}
    EventLoop* getLoop() const {
        return loop_;
    }
    void start();
    void handNewConn();
    void handThisConn() {
        loop_->updatePoller(acceptChannel_);
    }

private:
    EventLoop *loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
}; // Server

#endif // INCLUDE_SERVER_H
