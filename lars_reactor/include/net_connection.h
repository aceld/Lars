#pragma once

#include <stdio.h>

/*
 * 
 * 网络通信的抽象类，任何需要进行收发消息的模块，都可以实现该类
 *
 * */

class net_connection
{
public:
    net_connection():param(NULL) {}
    //发送消息的接口
    virtual int send_message(const char *data, int datalen, int msgid) = 0;
    virtual int get_fd() = 0;

    void *param; //TCP客户端可以 通过该参数传递一些自定义的参数
};

    //创建链接/销毁链接 要触发的 回调函数类型
typedef void (*conn_callback)(net_connection *conn, void *args);
