#pragma once

#include <pthread.h>

class Route
{
public:
    //创建单例的方法
    static void init() {
        _instance = new Route(); 
    }

    static Route *instance() {
        //保证init方法在这个进程执行中，只执行一次
        pthread_once(&_once, init);
        return _instance;
    }
    
private:
    //构造函数私有化
    Route();
    Route(const Route&);
    const Route& operator=(const Route&);

    //单例
    static Route* _instance;
    //单例锁
    static pthread_once_t _once;


    /* ---- 属性 ---- */
    //数据库句柄 
};
