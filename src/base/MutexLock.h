/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:30
 *   Filename        : MutexLock.h
 *   Description     : 
 * *******************************************************/

#ifndef INCLUDE_MUTEXLOCK_H
#define INCLUDE_MUTEXLOCK_H

#include <pthread.h>
#include <cstdio>
#include <boost/noncopyable.hpp>

class MutexLock : boost::noncopyable {
public:
    MutexLock() {
        pthread_mutex_init(&mutex, NULL);
    }
    ~MutexLock() {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }
    void lock() {
        pthread_mutex_lock(&mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_t *get() {
        return &mutex;
    }
private:
    pthread_mutex_t mutex;

// 友元类不受访问权限影响
private:
    friend class Condition;
};


class MutexLockGuard : boost::noncopyable {
public:
    explicit MutexLockGuard(MutexLock &_mutex):
    mutex(_mutex) {
        mutex.lock();
    }
    ~MutexLockGuard() {
        mutex.unlock();
    }
private:
    MutexLock &mutex;
}; // MutexLock

#endif // INCLUDE_MUTEXLOCK_H
