#pragma once

/*
 * 
 * 网络通信的抽象类，任何需要进行收发消息的模块，都可以实现该类
 *
 * */
class net_connection
{
public:

    //发送消息的接口
    virtual int send_data(const char *data, int datalen, int cmdid) = 0;

    //获取通信文件描述符的接口 
    virtual int get_fd() = 0;
};


