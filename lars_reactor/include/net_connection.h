#pragma once

/*
 * 
 * 网络通信的抽象类，任何需要进行收发消息的模块，都可以实现该类
 *
 * */

class net_connection
{
public:
    net_connection() {}
    //发送消息的接口
    virtual int send_message(const char *data, int datalen, int msgid) = 0;
};


