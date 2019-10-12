#pragma once
/*
 *
 * event_loop事件处理机制
 *
 * */
#include <sys/epoll.h>
#include <ext/hash_map>
#include <ext/hash_set>
#include <vector>
#include "event_base.h"

#include "task_msg.h"

#define MAXEVENTS 10

// map: fd->io_event 
typedef __gnu_cxx::hash_map<int, io_event> io_event_map;
//定义指向上面map类型的迭代器
typedef __gnu_cxx::hash_map<int, io_event>::iterator io_event_map_it;
//全部正在监听的fd集合
typedef __gnu_cxx::hash_set<int> listen_fd_set;

//定义异步任务回调函数类型
typedef void (*task_func)(event_loop *loop, void *args);

class event_loop 
{
public:
    //构造，初始化epoll堆
    event_loop();

    //阻塞循环处理事件
    void event_process();

    //添加一个io事件到loop中
    void add_io_event(int fd, io_callback *proc, int mask, void *args=NULL);

    //删除一个io事件从loop中
    void del_io_event(int fd);

    //删除一个io事件的EPOLLIN/EPOLLOUT
    void del_io_event(int fd, int mask);

    //获取全部监听事件的fd集合
    void get_listen_fds(listen_fd_set &fds) {
        fds = listen_fds;
    }


    //=== 异步任务task模块需要的方法 ===
    //添加一个任务task到ready_tasks集合中
    void add_task(task_func func, void *args);
    //执行全部的ready_tasks里面的任务
    void execute_ready_tasks();
    
private:
    int _epfd; //epoll fd

    //当前event_loop 监控的fd和对应事件的关系
    io_event_map _io_evs;

    //当前event_loop 一共哪些fd在监听
    listen_fd_set listen_fds;

    //一次性最大处理的事件
    struct epoll_event _fired_evs[MAXEVENTS];

    //需要被执行的task集合
    typedef std::pair<task_func, void*> task_func_pair;
    std::vector<task_func_pair> _ready_tasks;
};
