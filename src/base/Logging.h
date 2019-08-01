/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:31
 *   Filename        : Logging.h
 *   Description     : 
 *   Logging 是对外的接口，Logging 类包含一个 LogStream 对象，
 *   主要是为了每次打印 log 的时候和在 log 之前和之后奖赏固
 *   定的格式化信息，比如 log 所在行，文件名等信息
 *   __FILE__、__LINE__
 *
 * 多线程异步日志与单线程不同，需要进行线程安全的保证，即多
 * 个线程可以写日志文件而不发生错乱，简单的线程安全并不难
 * 办到，用一个全局的 mutex 对日志的 IO 进行保护或每个线程
 * 单独写一个日志文件就可以办到，但是前者会造成多个线程争夺
 * mutex，后者会造成业务线程阻塞在磁盘操作上
 * 本项目可行的解决方案就是，用一个背景线程负责收集日志
 * 消息并将其写入后端，其他业务线程只负责生成日志消息并将
 * 其传输到日志线程，这就是异步日志
 *
 * 在多线程服务器程序中，异步日志是必须的，如果在网络 IO 或
 * 业务线程中直接往磁盘上写数据的话，写操作可能由于某种原
 * 因阻塞达到数秒之久，这可能是的请求方超时，或者耽误心跳
 * 消息的发送，
 * *******************************************************/

/*
 * 日志模块调用顺序
 * Logger、lmpl、LogStream、operator<<、FixBuffer、g_output、append*/

#ifndef INCLUDE_LOGGING_H
#define INCLUDE_LOGGING_H

#include <pthread.h>
#include <string.h>
#include <string>
#include <stdio.h>

#include "LogStream.h"

class AsyncLogging;


class Logger {
public:
    Logger(const char *fileName, int line);
    ~Logger();
    LogStream& stream() { return impl_.stream_; }

    static void setLogFileName(std::string fileName) {
        logFileName_ = fileName;
    }
    static std::string getLogFileName() {
        return logFileName_;
    }

private:
    class Impl {
    public:
        Impl(const char *fileName, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
}; // Logger

// 日志打印宏
#define LOG Logger(__FILE__, __LINE__).stream()

#endif // INCLUDE_LOGGING_H
