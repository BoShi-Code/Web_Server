/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:34
 *   Filename        : LogFile.cpp
 *   Description     : 
 * *******************************************************/

#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "LogFile.h"
#include "FileUtil.h"

LogFile::LogFile(const std::string& basename, int flushEveryN)
    :basename_(basename),
    flushEveryN_(flushEveryN),
    count_(0),
    mutex_(new MutexLock)
{
    //assert(basename.find('/') >= 0);
    file_.reset(new AppendFile(basename));
}

LogFile::~LogFile() {}

void LogFile::append(const char* logline, int len) {
    MutexLockGuard lock(*mutex_);
    // std::lock_guard<std::mutex> lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    // std::lock_guard<std::mutex> lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) {
    file_->append(logline, len);
    ++count_;
    if (count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}
