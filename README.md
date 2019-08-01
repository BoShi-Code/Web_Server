# WebServer (学习项目)

[![](https://img.shields.io/badge/build-pass-brightgreen)](https://github.com/Apriluestc/web.d/edit/master/README.md)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
本项目为 C++11 编写的 Web 服务器，解析了 get、head 请求，可处理静态资源，支
持HTTP长连接，支持管线化请求，并实现了异步日志，记录服务器运行状态

## 技术特点

- 使用 Epoll 边沿触发的 IO 多路复用技术，非阻塞 IO，使用 Reactor 模式
- 使用多线程充分利用多核 CPU，并使用线程池避免线程频繁创建销毁的开销
- 使用基于小根堆的定时器关闭超时请求
- 使用状态机解析了 HTTP GET 和 HEAD 请求
- 主线程只负责 accept 请求，并以 Round Robin 的方式分发给其它 IO 线程(兼计算线程)，
锁的争用只会出现在主线程和某一特定线程中
- 使用保活机制实现了 HTTP 长连接
- 使用 eventfd 实现线程的异步唤醒
- 使用双缓冲区技术实现了简单的异步日志系统
- 为减少内存泄漏的可能，使用智能指针等 RAII 机制
- 支持优雅关闭连接

## 职责 (我实现了)

- 实现了基于时间轮的定时器，定时关闭超时请求以及剔除不活跃连接
- 实现了线程安全的高性能内存池 MemoryPool，减少频繁开辟和释放产生的内存碎片
- 解析了 HTTP POST 请求
- 实现了 TCP 接收发送缓冲区(环形缓冲区)
- 比较本项目与其他 Web 服务器的并发模型特点，如：nginx
- 使用 shell 脚本编写了 webd 服务的启动、关闭及重启(控制服务行为)
- 该服务以 dameon 进程运行

## [并发模型](https://github.com/Apriluestc/web.d/blob/master/%E5%B9%B6%E5%8F%91%E6%A8%A1%E5%9E%8B.md)

## 项目来源

- [陈硕](https://github.com/chenshuo/muduo/tree/master/muduo/net)
- 林亚
- 陈帅豪
