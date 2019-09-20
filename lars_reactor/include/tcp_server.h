#pragma once

#include <netinet/in.h>
#include "event_loop.h"
#include "tcp_conn.h"
#include "message.h"


class tcp_server
{ 
public: 
    //server的构造函数
    tcp_server(event_loop* loop, const char *ip, uint16_t port); 

    //开始提供创建链接服务
    void do_accept();

    //链接对象释放的析构
    ~tcp_server();

    //注册消息路由回调函数
    void add_msg_router(int msgid, msg_callback *cb, void *user_data = NULL) {
        router.register_msg_router(msgid, cb, user_data);
    }

private: 
    //基础信息
    int _sockfd; //套接字
    struct sockaddr_in _connaddr; //客户端链接地址
    socklen_t _addrlen; //客户端链接地址长度

    //event_loop epoll事件机制
    event_loop* _loop;


    //---- 客户端链接管理部分-----
public:
    static void increase_conn(int connfd, tcp_conn *conn);    //新增一个新建的连接
    static void decrease_conn(int connfd);    //减少一个断开的连接
    static void get_conn_num(int *curr_conn);     //得到当前链接的刻度
    static tcp_conn **conns;        //全部已经在线的连接信息

    //---- 消息分发路由 ----
    static msg_router router; 
private:
    //TODO 
    //从配置文件中读取
#define MAX_CONNS 10000
    static int _max_conns;          //最大client链接个数
    static int _curr_conns;         //当前链接刻度
    static pthread_mutex_t _conns_mutex; //保护_curr_conns刻度修改的锁
}; 
