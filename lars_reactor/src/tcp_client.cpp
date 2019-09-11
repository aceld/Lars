#include "tcp_client.h"
#include <strings.h>
#include <errno.h>




tcp_client::tcp_client(event_loop *loop, const char *ip, unsigned short port, const char *name)
{
    _sockfd = -1;
    _name = name;
    _loop = loop;
    
    bzero(&_server_addr, sizeof(_server_addr));
    
    _server_addr.sin_family = AF_INET; 
    inet_aton(ip, &_server_addr.sin_addr);
    _server_addr.sin_port = htons(port);

    _addrlen = sizeof(_server_addr);
}

//创建链接
void tcp_client::do_connect()
{
    if (_sockfd != -1) {
        close(_sockfd);
    }

    //创建套接字
    _sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if (_sockfd == -1) {
        fprintf(stderr, "create tcp client socket error\n");
        exit(1);
    }

    int ret = connect(_sockfd, (const struct sockaddr*)&_server_addr, _addrlen);
    if (ret == 0) {
        //链接创建成功  
        
        connected = true; 

        printf("connect %s:%d succ!\n", inet_ntoa(_server_addr.sin_addr), ntohs(_server_addr.sin_port));
    }
    else {
        if(errno == EINPROGRESS) {
            //fd是非阻塞的，可能会出现这个错误
            //TODO 

        }
        else {
            fprintf(stderr, "connection error\n");
            exit(1);
        }
    }

}


//主动发送message方法
int tcp_client::send_message(const char *data, int msglen, int msgid)
{
    return 0;
}


//处理读业务
void tcp_client::do_read()
{

}

//处理写业务
void tcp_client::do_write()
{

}


//释放链接资源
void tcp_client::clean_conn()
{

}


tcp_client::~tcp_client()
{

}
