# 并发模型

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
服务器项目使用 Reactor 模型，并使用多线程提高并发度，使用 epoll 作为 IO 多路复用的实现方式

## 并发模型

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
MainReactor 只有一个，负责响应 client 的连接请求，并建立连接，它使用一个 NIO Selector。在建立连接后用 Round Robin 的方式
分配给某个 SubReactor,因为涉及到跨线程任务分配，需要加锁，这里的锁由某个特定线程中的 loop 创建，只会被该线程和主线程竞争。

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
SubReactor 可以有一个或者多个，每个 subReactor 都会在一个独立线程中运行，并且维护一个独立的 NIO Selector。

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
当主线程把新连接分配给了某个 SubReactor，该线程此时可能正阻塞在多路选择器(epoll)的等待中，怎么得知新连接的到来呢？这里使
用了 eventfd 进行异步唤醒，线程会从 epoll_wait 中醒来，得到活跃事件，进行处理。

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
我学习了 muduo 库中的 runInLoop 和 queueInLoop 的设计方法，这两个方法主要用来执行用户的某个回调函数，queueInLoop 是
跨进程调用的精髓所在，具有极大的灵活性，我们只需要绑定好回调函数就可以了，我仿照muduo实现了这一点。

## epoll 工作模式

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
epoll 的触发模式我选择了 ET 模式，与 LT 有很大的不同，ET 要比 LT 复杂些许，它对用户提出更高的要求，即每次读，必须读到不能再读(出现 EAGAIN)，
写到不能再写的时候，而 LT 则简单许多，当然也可以这样做，选择 LT，每次只 read 一次 OK，这样可以减少系统调用的次数

## 定时器

- 方案1：一个定时器，一个 mulimap<endtime, request> 保存请求超时列表，每次超时检查 mulimap，这样，
请求的插入时间复杂度 O(logn)，遍历和删除 O(1)，且需要额外编码
- 方案2：一个请求一个定时器，如此无需额外开销保存请求，无需额外编码，等待超时处理请求即可，时间复杂度 0

基于时间轮实现定时器，用于处理超时请求和不活跃连接，除此之外
- 基于排序链表    StartTimer O(n)    StopTimer O(1)    PerTickBookkeeping O(1)
- 基于小根堆      StartTimer O(logn) StopTimer O(1)    PerTickBookkeeping O(1)
- 基于时间轮      StartTimer O(1)    StopTimer O(1)    PerTickBookkeeping O(1)

## 日志系统

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
多线程异步日志需要进行线程安全的保证，即多个线程可以写日志文件而不发生错乱，简单
的线程安全并不难办到，用一个全局的 Mutex 对日志的 IO 操作进行保护或者单独写一个日志
文件就可以办到，但是前者会造成多个线程争夺锁资源 Mutex，后者可能会造成业务线程阻塞在磁
盘操作上

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
本项目可行的解决方案就是，用一个背景线程负责收集日志消息并将其写入后端，其他业务线程只
负责生成日志消息并将其传输到日志线程，即异步日志

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
日志的实现应用双缓冲区技术，即存在两个 Buffer，日志的实现分为前端和后端，
前端负责向 CurrentBuffer 中写，后端负责将其写入文件中，具体来说，当
CurrentBuffer 写满时，先将 CurrentBuffer 中的消息存入 Buffer 中，在交换
CurrentBuffer 和 NextBuffer，这样前端就可以继续往 CurrentBuffer 中写新的日志消息，
最后再调用 notify_all 通知后端将其写入文件

### Log 具体实现

```html
src/base/
├── FileUtil : 封装了 log 文件的打开、写入、关闭，底层使用了标准 IO
├── LogFile : 进一步向上封装 FileUtil，并设置 Buffer 刷新 TIME
├── AsyncLogging : 负责启动 log 线程，专门将日志写入文件，应用双缓冲区技术，AsyncLogging 负责将缓冲区数据写入日志文件
├── Logging : 提供日志系统对外接口：SetLogFileName，写文件底层实现均以 append 展开
└── LogStream : 主要用于格式化输出日志消息，重载流 << 运算符
```
