/**********************************************************
 *   Author          : Apriluestc
 *   Email           : 13669186256@163.com
 *   Last modified   : 2019-07-28 13:23
 *   Filename        : Main.cpp
 *   Description     : 
 * *******************************************************/

#include <getopt.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"

int main(int argc, char *argv[])
{
    mkdir("log", 0775);
    int threadNum = 4;
    int port = 8000;
    std::string logPath = "./log/webd.log";

    // parse args
    int opt;
    const char *str = "t:l:p:";
    while ((opt = getopt(argc, argv, str))!= -1) {
        switch (opt) {
            case 't':
            {
                threadNum = atoi(optarg);
                break;
            }
            case 'l':
            {
                logPath = optarg;
                if (logPath.size() < 2 || optarg[0] != '/') {
                    printf("logPath should start with \"/\"\n");
                    abort();
                }
                break;
            }
            case 'p':
            {
                port = atoi(optarg);
                break;
            }
            default: break;
        }
    }
    Logger::setLogFileName(logPath);
    // STL库在多线程上应用
    #ifndef _PTHREADS
        LOG << "_PTHREADS is not defined !";
    #endif
    EventLoop mainLoop;
    daemon(1, 0);
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start();
    mainLoop.loop();
    return 0;
}
