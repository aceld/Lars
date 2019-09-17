#pragma once

#include "io_buf.h"
#include "event_loop.h"
#include "message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class tcp_client
{
public:
    //初始化客户端套接字
    tcp_client(event_loop *loop, const char *ip, unsigned short port,  const char *name);

    //发送message方法
    int send_message(const char *data, int msglen, int msgid);

    //创建链接
    void do_connect();

    //处理读业务
    int do_read();
    
    //处理写业务
    int do_write();
    
    //释放链接资源
    void clean_conn();

    ~tcp_client();


    //设置业务处理回调函数
    void set_msg_callback(msg_callback *msg_cb) 
    {
        this->_msg_callback = msg_cb;
    }

    bool connected; //链接是否创建成功
    //server端地址
    struct sockaddr_in _server_addr;
    io_buf _obuf;
    io_buf _ibuf;

private:
    int _sockfd;
    socklen_t _addrlen;

    //客户端的事件处理机制
    event_loop* _loop;

    //当前客户端的名称 用户记录日志
    const char *_name;

    msg_callback *_msg_callback;
};
