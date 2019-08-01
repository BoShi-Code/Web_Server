/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:34
 *   Filename        : FileUtil.h
 *   Description     : 
 *
 *   FileUtil 是最底层的文件操作类，封装了 Log 文件的打开、
 *   写入并在析构函数中关闭文件，底层使用了标准 IO，使
 *   用 append 函数向文件中写
 * *******************************************************/

#ifndef INCLUDE_FILEUTIL_H
#define INCLUDE_FILEUTIL_H

#include <boost/noncopyable.hpp>
#include <string>

class AppendFile : boost::noncopyable {
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();

    // append 会向文件写
    // 核心函数，写操作
    void append(const char *logline, const size_t len);
    void flush();

private:
    size_t write(const char *logline, size_t len);
    FILE* fp_;
    char buffer_[64*1024];
}; // AppendFile

#endif // INCLUDE_FILEUTIL_H
