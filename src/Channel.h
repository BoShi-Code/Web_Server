/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:06
 *   Filename        : Channel.h
 *   Description     : Channel 类，文件描述符和事件的封装
 * *******************************************************/

#ifndef INCLUDE_CHANNEL_H
#define INCLUDE_CHANNEL_H

#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>
#include <sys/epoll.h>

#include "Timer.h"

class EventLoop;
class HttpData;

class Channel {
private:
    // 回调函数类型
    typedef std::function<void()> CallBack;
    EventLoop *loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents_;

    // 方便找到上层持有该Channel的对象
    std::weak_ptr<HttpData> holder_;

private:
    int parse_URI();
    int parse_Headers();
    int analysisRequest();

    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;

public:
    Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
    ~Channel();

    // 获取文件描述符
    int getFd();

    // 设置文件描述符
    void setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder) {
        holder_ = holder;
    }
    std::shared_ptr<HttpData> getHolder() {
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }

    // 读事件回调
    void setReadHandler(CallBack &&readHandler) {
        readHandler_ = readHandler;
    }

    // 写事件回调
    void setWriteHandler(CallBack &&writeHandler) {
        writeHandler_ = writeHandler;
    }

    // 错误事件回调
    void setErrorHandler(CallBack &&errorHandler) {
        errorHandler_ = errorHandler;
    }

    // 连接异常关闭，关闭事件回调
    void setConnHandler(CallBack &&connHandler) {
        connHandler_ = connHandler;
    }

    // 读、写、异常、关闭事件分发处理
    void handleEvents() {
        events_ = 0;

        // EPOLLHUP
        // EPOLLIN 表示对应文件描述符可读(包括对端 socket 正常关闭)
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            events_ = 0;
            return;
        }

        // EPOLLERR 表示对应文件描述符发生错误
        if (revents_ & EPOLLERR) {
            if (errorHandler_) 
                errorHandler_();
            events_ = 0;
            return;
        }

        // EPOLLRDHUP 表示对应文件描述符
        // EPOLLPRI 表示对应文件描述符有紧急数据可读(这里表示对外数据到来)
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        
        // EPOLLOUT 表示对应文件描述符可写
        if (revents_ & EPOLLOUT) {
            handleWrite();
        }
        handleConn();
    }

    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();

    void setRevents(__uint32_t ev) {
        revents_ = ev;
    }

    // 设置触发事件
    void setEvents(__uint32_t ev) {
        events_ = ev;
    }
    
    // 获取触发事件
    __uint32_t& getEvents() {
        return events_;
    }

    bool EqualAndUpdateLastEvents() {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }

    __uint32_t getLastEvents() {
        return lastEvents_;
    }

}; // Channel

typedef std::shared_ptr<Channel> SP_Channel;

#endif // INCLUDE_CHANNEL_H
