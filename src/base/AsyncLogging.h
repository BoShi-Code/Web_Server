/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:26
 *   Filename        : AsyncLogging.h
 *   Description     : 
 *   AsyncLogging 是核心，负责启动 log 线程，专门将日志写入
 *   文件，应用双缓冲区技术，AsyncLogging 负责将缓冲区数
 *   据写入日志文件中
 *
 *   前端如何将日志消息传输到后端
 *   双缓冲区技术即预先设置两个 buffer，前端负责向 currentBuffer 中
 *   写日志，后端负责将其写入日志文件，具体来说，当 currentBuffer 写
 *   满时，先将 currentBuffer 中消息存入 buffer 中，再交换
 *   currentBuffer 和 nextBuffer，这样前端就可以继续
 *   往 currentBuffer 中写入新的日志消息，最后再调
 *   用 notify_all 通知后端日志线程，将其写入日志文件
 * *******************************************************/

#ifndef INCLUDE_ASYNCLOGGING_H
#define INCLUDE_ASYNCLOGGING_H

#include <functional>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

#include "CountDownLatch.h"
#include "MutexLock.h"
#include "Thread.h"
#include "LogStream.h"

class AsyncLogging {
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging() {
        if (running_)
            stop();
    }
    void append(const char* logline, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

public:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;
    const int flushInterval_;
    bool running_;
    std::string basename_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;

    // 当前缓冲区 currentBuffer
    BufferPtr currentBuffer_;

    // 预备缓冲区
    BufferPtr nextBuffer_;

    // 待写入文件的已经填满的缓冲，供后端写入的 Buffer
    BufferVector buffers_;
    CountDownLatch latch_;
}; // AsyncLogging

#endif // INCLUDE_ASYNCLOGGING_H
