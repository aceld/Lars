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
#include "reactor_buf.h"



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
            
            int ret = 0;
            input_buf ibuf;
            output_buf obuf;

            char *msg = NULL;
            int msg_len = 0;
            do { 
                ret = ibuf.read_data(connfd);
                if (ret == -1) {
                    fprintf(stderr, "ibuf read_data error\n");
                    break;
                }
                printf("ibuf.length() = %d\n", ibuf.length());

                
                //将读到的数据放在msg中
                msg_len = ibuf.length();
                msg = (char*)malloc(msg_len);
                bzero(msg, msg_len);
                memcpy(msg, ibuf.data(), msg_len);
                ibuf.pop(msg_len);
                ibuf.adjust();

                printf("recv data = %s\n", msg);

                //回显数据
                obuf.send_data(msg, msg_len);
                while(obuf.length()) {
                    int write_ret = obuf.write2fd(connfd);
                    if (write_ret == -1) {
                        fprintf(stderr, "write connfd error\n");
                        return;
                    }
                    else if(write_ret == 0) {
                        //不是错误，表示此时不可写
                        break;
                    }
                }
                 

                free(msg);
                    
            } while (ret != 0);     


            //Peer is closed
            close(connfd);
        }
    }
}

//链接对象释放的析构
tcp_server::~tcp_server()
{
    close(_sockfd);
}
