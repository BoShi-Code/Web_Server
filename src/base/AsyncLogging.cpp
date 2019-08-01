/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:26
 *   Filename        : AsyncLogging.cpp
 *   Description     : 
 * *******************************************************/

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <functional>

#include "AsyncLogging.h"
#include "LogFile.h"

AsyncLogging::AsyncLogging(std::string logFileName_,int flushInterval)
  : flushInterval_(flushInterval),
    running_(false),
    basename_(logFileName_),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(),
    latch_(1)
{
    assert(logFileName_.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline, int len) {
    MutexLockGuard lock(mutex_);
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_.reset(new Buffer);
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown();

    // 直接进行 IO 的日志文件
    LogFile output(basename_);

    // 后端准备两个 Buffer，预防临界区(超时、currentBuffer 写满)
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();

    // 用来和前端线程交换 Buffer
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_) {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty()) {
                cond_.waitForSeconds(flushInterval_);
            }

            /*
             * 无论 cond 是因何种原因醒来，都要将 currentBuffer 放到 buffers 中。如果是因为时间
             * 到而醒，那么就算 currentBuffer 还没写满，此时也要将它写入 LogFile 中。如果前台
             * currentBuffer 已经满了，那么在前台线程中就已经把一个前台 currentBuffer 放到
             * buffers 中了。此时，还是需要把 currentBuffer 放到 buffers 中（注意，前后放置是不同
             * 的 buffer，因为在前台线程中，currentBuffer 已经被换成 nextBuffer 指向的buffer了）
             * */
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25) {
            //char buf[256];
            // snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
            //          Timestamp::now().toFormattedString().c_str(),
            //          buffersToWrite.size()-2);
            //fputs(buf, stderr);
            //output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        // 将已经写满的 Buffer 写入到日志文件中，由 LogFile 进行 IO 操作
        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
