/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:28
 *   Filename        : CountDownLatch.h
 *   Description     : 
 * *******************************************************/
#ifndef INCLUDE_COUNTDOWNLATCH_H
#define INCLUDE_COUNTDOWNLATCH_H

#include <boost/noncopyable.hpp>

#include "Condition.h"
#include "MutexLock.h"

// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : boost::noncopyable {
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
}; // CountDownLatch

#endif // INCLUDE_COUNTDOWNLATCH_H
