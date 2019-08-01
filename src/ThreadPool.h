/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:15
 *   Filename        : ThreadPool.h
 *   Description     : 线程池实现，表示工作线程，执行通用任务
 * *******************************************************/

#ifndef INCLUDE_THREADPOOL_H
#define INCLUDE_THREADPOOL_H

#include <pthread.h>
#include <functional>
#include <memory>
#include <vector>

#include "Channel.h"

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;

typedef enum {
    immediate_shutdown = 1,
    graceful_shutdown  = 2
} ShutDownOption;

struct ThreadPoolTask {
    std::function<void(std::shared_ptr<void>)> fun;
    std::shared_ptr<void> args;
};


class ThreadPool {
private:
    // 任务队列互斥量
    static pthread_mutex_t lock;

    // 任务队列同步的条件变量
    static pthread_cond_t notify;

    // 线程数组
    static std::vector<pthread_t> threads;

    // 任务队列
    // 用条件变量同步任务队列
    static std::vector<ThreadPoolTask> queue;

    // 线程数量
    static int thread_count;

    // 任务队列大小
    static int queue_size;
    static int head;
    // tail 指向尾节点的下一节点
    static int tail;
    static int count;
    static int shutdown;

    // 开启线程
    static int started;
public:

    // 创建线程
    static int threadpool_create(int _thread_count, int _queue_size);

    // 添加任务
    static int threadpool_add(std::shared_ptr<void> args, std::function<void(std::shared_ptr<void>)> fun);

    // 销毁线程
    static int threadpool_destroy(ShutDownOption shutdown_option = graceful_shutdown);
    

    // 销毁线程池
    static int threadpool_free();

    // 创建线程池
    static void *threadpool_thread(void *args);
}; // ThreadPool

#endif // INCLUDE_THREADPOOL_H
