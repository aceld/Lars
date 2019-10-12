#pragma once

#include <pthread.h>
#include "task_msg.h"
#include "thread_queue.h"

class thread_pool
{
public:
    //构造，初始化线程池, 开辟thread_cnt个
    thread_pool(int thread_cnt);

    //获取一个thead
    thread_queue<task_msg>* get_thread();

    //发送一个task任务给thread_pool里的全部thread
    void send_task(task_func func, void *args = NULL);
private:

    //_queues是当前thread_pool全部的消息任务队列头指针
    thread_queue<task_msg> ** _queues; 

    //当前线程池中的线程个数
    int _thread_cnt;

    //已经启动的全部therad编号
    pthread_t * _tids;

    //当前选中的线程队列下标
    int _index;
};
