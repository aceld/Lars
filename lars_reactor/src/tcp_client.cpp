#include "tcp_client.h"
#include "message.h"
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>


static void write_callback(event_loop *loop, int fd, void *args)
{
    tcp_client *cli = (tcp_client *)args;
    cli->do_write();
}

static void read_callback(event_loop *loop, int fd, void *args)
{
    tcp_client *cli = (tcp_client *)args;
    cli->do_read();
}

tcp_client::tcp_client(event_loop *loop, const char *ip, unsigned short port, const char *name):
    _conn_start_cb(NULL),
    _conn_start_cb_args(NULL),
    _conn_close_cb(NULL),
    _conn_close_cb_args(NULL),
    _obuf(4194304),
    _ibuf(4194304)
{
    _sockfd = -1;
    _name = name;
    _loop = loop;
    
    bzero(&_server_addr, sizeof(_server_addr));
    
    _server_addr.sin_family = AF_INET; 
    inet_aton(ip, &_server_addr.sin_addr);
    _server_addr.sin_port = htons(port);

    _addrlen = sizeof(_server_addr);

    this->do_connect();
}

int tcp_client::get_fd()
{
    return _sockfd;
}

//判断链接是否是创建链接，主要是针对非阻塞socket 返回EINPROGRESS错误
static void connection_delay(event_loop *loop, int fd, void *args)
{
    tcp_client *cli = (tcp_client*)args;
    loop->del_io_event(fd);

    int result = 0;
    socklen_t result_len = sizeof(result);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len);
    if (result == 0) {
        //链接是建立成功的
        cli->connected = true;

        printf("connect %s:%d succ!\n", inet_ntoa(cli->_server_addr.sin_addr), ntohs(cli->_server_addr.sin_port));

        //调用开发者注册的创建链接Hook函数
        if (cli->_conn_start_cb != NULL) {
            cli->_conn_start_cb(cli, cli->_conn_start_cb_args);
        }

        loop->add_io_event(fd, read_callback, EPOLLIN, cli);

        if (cli->_obuf.length != 0) {
            //输出缓冲有数据可写
            loop->add_io_event(fd, write_callback, EPOLLOUT, cli);
        }
    }
    else {
        //链接创建失败
        fprintf(stderr, "connection %s:%d error\n", inet_ntoa(cli->_server_addr.sin_addr), ntohs(cli->_server_addr.sin_port));
    }
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

        //调用开发者客户端注册的创建链接之后的hook函数
        if (_conn_start_cb != NULL) {
            _conn_start_cb(this, _conn_start_cb_args);
        }
        

        //注册读回调
        _loop->add_io_event(_sockfd, read_callback, EPOLLIN, this);
        //如果写缓冲去有数据，那么也需要触发写回调
        if (this->_obuf.length != 0) {
            _loop->add_io_event(_sockfd, write_callback, EPOLLOUT, this);
        }
        
        
        printf("connect %s:%d succ!\n", inet_ntoa(_server_addr.sin_addr), ntohs(_server_addr.sin_port));
    }
    else {
        if(errno == EINPROGRESS) {
            //fd是非阻塞的，可能会出现这个错误,但是并不表示链接创建失败
            //如果fd是可写状态，则为链接是创建成功的.
            fprintf(stderr, "do_connect EINPROGRESS\n");

            //让event_loop去触发一个创建判断链接业务 用EPOLLOUT事件立刻触发
            _loop->add_io_event(_sockfd, connection_delay, EPOLLOUT, this);
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
    if (connected == false) {
        fprintf(stderr, "no connected , send message stop!\n");
        return -1;
    }

    //是否需要添加写事件触发
    //如果obuf中有数据，没必要添加，如果没有数据，添加完数据需要触发
    bool need_add_event = (_obuf.length == 0) ? true:false;
    if (msglen + MESSAGE_HEAD_LEN > this->_obuf.capacity - _obuf.length) {
        fprintf(stderr, "No more space to Write socket!\n");
        return -1;
    }

    //封装消息头
    msg_head head;
    head.msgid = msgid;
    head.msglen = msglen;

    memcpy(_obuf.data + _obuf.length, &head, MESSAGE_HEAD_LEN);
    _obuf.length += MESSAGE_HEAD_LEN;

    memcpy(_obuf.data + _obuf.length, data, msglen);
    _obuf.length += msglen;

    if (need_add_event) {
        _loop->add_io_event(_sockfd, write_callback, EPOLLOUT, this);
    }

    return 0;
}


//处理读业务
int tcp_client::do_read()
{
    //确定已经成功建立连接
    assert(connected == true);
    // 1. 一次性全部读取出来
    
    //得到缓冲区里有多少字节要被读取，然后将字节数放入b里面。   
    int need_read = 0;
    if (ioctl(_sockfd, FIONREAD, &need_read) == -1) {
        fprintf(stderr, "ioctl FIONREAD error");
        return -1;
    }


    //确保_buf可以容纳可读数据
    assert(need_read <= _ibuf.capacity - _ibuf.length);

    int ret;

    do {
        ret = read(_sockfd, _ibuf.data + _ibuf.length, need_read);
    } while(ret == -1 && errno == EINTR);

    if (ret == 0) {
        //对端关闭
        if (_name != NULL) {
            printf("%s client: connection close by peer!\n", _name);
        }
        else {
            printf("client: connection close by peer!\n");
        }

        clean_conn();
        return -1;
    }
    else if (ret == -1) {
        fprintf(stderr, "client: do_read() , error\n");
        clean_conn();
        return -1;
    }

    
    assert(ret == need_read);
    _ibuf.length += ret;

    //2. 解包
    msg_head head;
    int msgid, length;
    while (_ibuf.length >= MESSAGE_HEAD_LEN) {
        memcpy(&head, _ibuf.data + _ibuf.head, MESSAGE_HEAD_LEN);
        msgid = head.msgid; 
        length = head.msglen;

        /*
        if (length + MESSAGE_HEAD_LEN < _ibuf.length) {
            break;
        }
        */

        //头部读取完毕
        _ibuf.pop(MESSAGE_HEAD_LEN);

        //3. 消息路由分发
        this->_router.call(msgid, length, _ibuf.data + _ibuf.head, this);

        //数据区域处理完毕
        _ibuf.pop(length);
    }
    
    //重置head指针
    _ibuf.adjust();

    return 0;
}

//处理写业务
int tcp_client::do_write()
{
    //数据有长度，切头部索引是起始位置
    assert(_obuf.head == 0 && _obuf.length);

    int ret;

    while (_obuf.length) {
        //写数据
        do {
            ret = write(_sockfd, _obuf.data, _obuf.length);
        } while(ret == -1 && errno == EINTR);//非阻塞异常继续重写

        if (ret > 0) {
           _obuf.pop(ret);
           _obuf.adjust();
        } 
        else if (ret == -1 && errno != EAGAIN) {
            fprintf(stderr, "tcp client write \n");
            this->clean_conn();
        }
        else {
            //出错,不能再继续写
            break;
        }
    }

    if (_obuf.length == 0) {
        //已经写完，删除写事件
        //printf("do write over, del EPOLLOUT\n");
        this->_loop->del_io_event(_sockfd, EPOLLOUT);
    }

    return 0;
}


//释放链接资源,重置连接
void tcp_client::clean_conn()
{
    if (_sockfd != -1) {
        printf("clean conn, del socket!\n");
        _loop->del_io_event(_sockfd);
        close(_sockfd);
    }

    connected = false;

    //调用开发者注册的销毁链接之前触发的Hook
    if (_conn_close_cb != NULL) {
        _conn_close_cb(this, _conn_close_cb_args);
    }

    //重新连接
    this->do_connect();
}


tcp_client::~tcp_client()
{
    close(_sockfd);
}
