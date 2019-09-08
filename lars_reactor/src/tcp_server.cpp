#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "tcp_server.h"


//server的构造函数
tcp_server::tcp_server(const char *ip, uint16_t port)
{
    bzero(&_connaddr, sizeof(_connaddr));
    
    //忽略一些信号 SIGHUP, SIGPIPE
    //SIGPIPE:如果客户端关闭，服务端再次write就会产生
    //SIGHUP:如果terminal关闭，会给当前进程发送该信号
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "signal ignore SIGHUP\n");
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "signal ignore SIGPIPE\n");
    }

    //1. 创建socket
    _sockfd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/ | SOCK_CLOEXEC, IPPROTO_TCP);
    if (_sockfd == -1) {
        fprintf(stderr, "tcp_server::socket()\n");
        exit(1);
    }

    //2 初始化地址
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    //2-1可以多次监听，设置REUSE属性
    int op = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        fprintf(stderr, "setsocketopt SO_REUSEADDR\n");
    }

    //3 绑定端口
    if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "bind error\n");
        exit(1);
    }

    //4 监听ip端口
    if (listen(_sockfd, 500) == -1) {
        fprintf(stderr, "listen error\n");
        exit(1);
    }
}

//开始提供创建链接服务
void tcp_server::do_accept()
{
    int connfd;    
    while(true) {
        //accept与客户端创建链接
        printf("begin accept\n");
        connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);
        if (connfd == -1) {
            if (errno == EINTR) {
                fprintf(stderr, "accept errno=EINTR\n");
                continue;
            }
            else if (errno == EMFILE) {
                //建立链接过多，资源不够
                fprintf(stderr, "accept errno=EMFILE\n");
            }
            else if (errno == EAGAIN) {
                fprintf(stderr, "accept errno=EAGAIN\n");
                break;
            }
            else {
                fprintf(stderr, "accept error");
                exit(1);
            }
        }
        else {
            //accept succ!
            //TODO 添加心跳机制
            
            //TODO 消息队列机制
            
            int writed;
            const char *data = "hello Lars\n";
            do {
                writed = write(connfd, data, strlen(data)+1);
            } while (writed == -1 && errno == EINTR);

            if (writed > 0) {
                //succ
                printf("write succ!\n");
            }
            if (writed == -1 && errno == EAGAIN) {
                writed = 0; //不是错误，仅返回0表示此时不可继续写
            }
        }
    }
}

//链接对象释放的析构
tcp_server::~tcp_server()
{
    close(_sockfd);
}
