#include <signal.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "udp_server.h"


void read_callback(event_loop *loop, int fd, void *args)
{
    udp_server *server = (udp_server*)args;

    //处理业务函数
    server->do_read();
}

void udp_server::do_read()
{
    while (true) {
        int pkg_len = recvfrom(_sockfd, _read_buf, sizeof(_read_buf), 0, (struct sockaddr *)&_client_addr, &_client_addrlen);

        if (pkg_len == -1) {
            if (errno == EINTR) {
                continue;
            }
            else if (errno == EAGAIN) {
                break;
            }
            else {
                perror("recvfrom\n");
                break;
            }
        }

        //处理数据
        msg_head head; 
        memcpy(&head, _read_buf, MESSAGE_HEAD_LEN);
        if (head.msglen > MESSAGE_LENGTH_LIMIT || head.msglen < 0 || head.msglen + MESSAGE_HEAD_LEN != pkg_len) {
            //报文格式有问题
            fprintf(stderr, "do_read, data error, msgid = %d, msglen = %d, pkg_len = %d\n", head.msgid, head.msglen, pkg_len);
            continue;
        }

        //调用注册的路由业务
        _router.call(head.msgid, head.msglen, _read_buf+MESSAGE_HEAD_LEN, this);
    }
}


udp_server::udp_server(event_loop *loop, const char *ip, uint16_t port)
{
    //1 忽略一些信号
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGHUB");
        exit(1);
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal ignore SIGPIPE");
        exit(1);
    }
    
    //2 创建套接字
    //SOCK_CLOEXEC在execl中使用该socket则关闭，在fork中使用该socket不关闭
    _sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
    if (_sockfd == -1) {
        perror("create udp socket");
        exit(1);
    }

    //3 设置服务ip+port
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(ip, &servaddr.sin_addr);//设置ip
    servaddr.sin_port = htons(port);//设置端口

    //4 绑定
    bind(_sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    
    //3 添加读业务事件
    _loop = loop;

    bzero(&_client_addr, sizeof(_client_addr));
    _client_addrlen = sizeof(_client_addr);
    

    printf("server on %s:%u is running...\n", ip, port);

    _loop->add_io_event(_sockfd, read_callback, EPOLLIN, this);
    
}

int udp_server::send_message(const char *data, int msglen, int msgid)
{
    if (msglen > MESSAGE_LENGTH_LIMIT) {
        fprintf(stderr, "too large message to send\n");
        return -1;
    }

    msg_head head;
    head.msglen = msglen;
    head.msgid = msgid;

    memcpy(_write_buf,  &head, MESSAGE_HEAD_LEN);
    memcpy(_write_buf + MESSAGE_HEAD_LEN, data, msglen);

    int ret = sendto(_sockfd, _write_buf, msglen + MESSAGE_HEAD_LEN, 0, (struct sockaddr*)&_client_addr, _client_addrlen);
    if (ret == -1) {
        perror("sendto()..");
        return -1;
    }

    return ret;
}

//注册消息路由回调函数
void udp_server::add_msg_router(int msgid, msg_callback* cb, void *user_data)
{
    _router.register_msg_router(msgid, cb, user_data);
}

udp_server::~udp_server()
{
    _loop->del_io_event(_sockfd);
    close(_sockfd);
}
