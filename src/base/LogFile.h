/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:33
 *   Filename        : LogFile.h
 *   Description     : 
 *   LogFile 进一步封装了 FileUtil，并设置了循环次数，决定
 *   flush 间隔
 * *******************************************************/

#ifndef INCLUDE_LOGFILE_H
#define INCLUDE_LOGFILE_H

#include <memory>
#include <mutex>
#include <string>
#include <boost/noncopyable.hpp>

#include "FileUtil.h"
#include "MutexLock.h"

class LogFile : boost::noncopyable {
public:
    // 每被append flushEveryN次，flush一下，会往文件写，只不过，文件也是带缓冲区的
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    // 写文件 append
    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    // std::unique_ptr<std::mutex> mutex_;
    std::unique_ptr<AppendFile> file_;
}; // LogFile

#endif // INCLUDE_LOGFILE_H
