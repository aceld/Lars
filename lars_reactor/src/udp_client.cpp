
#include "udp_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>


void read_callback(event_loop *loop, int fd, void *args)
{
    udp_client *client = (udp_client*)args;
    client->do_read();
}



udp_client::udp_client(event_loop *loop, const char *ip, uint16_t port)
{
    //1 创建套接字
    _sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
    if (_sockfd == -1) {
        perror("create socket error");
        exit(1);
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    //2 链接
    int ret = connect(_sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret  == -1) {
        perror("connect");
        exit(1);
    }


    //3 添加读事件
    _loop = loop; 
    _loop->add_io_event(_sockfd, read_callback, EPOLLIN, this);
}

int udp_client::get_fd()
{
    return _sockfd;
}

udp_client::~udp_client()
{
    _loop->del_io_event(_sockfd);
    close(_sockfd);
}

//处理消息
void udp_client::do_read()
{
    while (true) {
        int pkt_len = recvfrom(_sockfd, _read_buf, sizeof(_read_buf), 0, NULL, NULL);
        if (pkt_len == -1) {
            if (errno == EINTR) {
                continue;
            }
            else if (errno == EAGAIN) {
                break;
            }
            else {
                perror("recvfrom()");
                break;
            }
        }

        //处理客户端包
        msg_head head; 
        memcpy(&head, _read_buf, MESSAGE_HEAD_LEN);
        if (head.msglen > MESSAGE_LENGTH_LIMIT || head.msglen < 0 || head.msglen + MESSAGE_HEAD_LEN != pkt_len) {
            //报文格式有问题
            fprintf(stderr, "do_read, data error, msgid = %d, msglen = %d, pkt_len = %d\n", head.msgid, head.msglen, pkt_len);
            continue;
        }

        //调用注册的路由业务
        _router.call(head.msgid, head.msglen, _read_buf+MESSAGE_HEAD_LEN, this);
    }
}
    
void udp_client::add_msg_router(int msgid, msg_callback *cb, void *user_data)
{
    _router.register_msg_router(msgid, cb, user_data);
}

int udp_client::send_message(const char *data, int msglen, int msgid)
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

    int ret = sendto(_sockfd, _write_buf, msglen + MESSAGE_HEAD_LEN, 0, NULL, 0);
    if (ret == -1) {
        perror("sendto()..");
        return -1;
    }

    return ret;
}


