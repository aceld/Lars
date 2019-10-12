#pragma  once

#include "event_loop.h"

//定义异步任务回调函数类型
typedef void (*task_func)(event_loop* loop, void *args);

struct task_msg
{
    enum TASK_TYPE
    {
        NEW_CONN,   //新建链接的任务
        NEW_TASK,   //一般的任务
    };

    TASK_TYPE type; //任务类型

    //任务的一些参数
    union {
        //针对 NEW_CONN新建链接任务，需要传递connfd
        int connfd;


        //针对 NEW_TASK 新建任务, 
        //可以给一个任务提供一个回调函数
        struct {
            task_func task_cb; //注册的任务函数
            void *args;        //任务函数对应的形参
        };
    };
};
