#pragma once

#include "reactor_buf.h"
#include "event_loop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class tcp_client
{
public:
    //初始化客户端套接字
    tcp_client(event_loop *loop, const char *ip, unsigned short port,  const char *name);

    //主动发送message方法
    int send_message(const char *data, int msglen, int msgid);

    //创建链接
    void do_connect();

    //处理读业务
    void do_read();
    
    //处理写业务
    void do_write();
    
    //释放链接资源
    void clean_conn();

    ~tcp_client();

private:
    int _sockfd;

    //server端地址
    struct sockaddr_in _server_addr;
    socklen_t _addrlen;

    output_buf obuf;
    input_buf  ibuf;
        

    //客户端的事件处理机制
    event_loop* _loop;

    //当前客户端的名称 用户记录日志
    const char *_name;

    bool connected; //链接是否创建成功
};
