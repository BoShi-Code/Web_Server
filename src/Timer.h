/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:16
 *   Filename        : Timer.h
 *   Description     : 基于小根堆的定时器实现
 *   定时剔除不活跃连接，和死连接
 * *******************************************************/

#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H

#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>

#include "base/MutexLock.h"

class HttpData;

class TimerNode {
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode &tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted() { deleted_ = true; }
    bool isDeleted() const { return deleted_; }
    size_t getExpTime() const { return expiredTime_; } 

private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> SPHttpData;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const {
        return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager {
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
    //MutexLock lock;
}; // Timer

#endif // INCLUDE_TIMER_H
