#pragma once

#include "net_connection.h"
#include <ext/hash_map>

//解决tcp粘包问题的消息头
struct msg_head
{
    int msgid;
    int msglen;
};

//消息头的二进制长度，固定数
#define MESSAGE_HEAD_LEN 8

//消息头+消息体的最大长度限制
#define MESSAGE_LENGTH_LIMIT (65535 - MESSAGE_HEAD_LEN)

//msg 业务回调函数原型

typedef void msg_callback(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data);


//消息路由分发机制
class msg_router 
{
public:
    msg_router():_router(),_args() {
        //printf("msg_router init...\n");
        printf(" \n");
    }  

    //给一个消息ID注册一个对应的回调业务函数
    int register_msg_router(int msgid, msg_callback *msg_cb, void *user_data) 
    {
        if(_router.find(msgid) != _router.end()) {
            //该msgID的回调业务已经存在
            return -1;
        }
        printf("add msg cb msgid = %d\n", msgid);

        _router[msgid] = msg_cb;
        _args[msgid] = user_data;

        return 0;
    }

    //调用注册的对应的回调业务函数
    void call(int msgid, uint32_t msglen, const char *data, net_connection *net_conn) 
    {
        //printf("call msgid = %d\n", msgid);
        //printf("call data = %s\n", data);
        //printf("call msglen = %d\n", msglen);
        //判断msgid对应的回调是否存在
        if (_router.find(msgid) == _router.end()) {
            fprintf(stderr, "msgid %d is not register!\n", msgid);
            return;
        }

        //直接取出回调函数，执行
        msg_callback *callback = _router[msgid];
        void *user_data = _args[msgid];
        callback(data, msglen, msgid, net_conn, user_data);
    }

private:
    //针对消息的路由分发，key为msgID, value为注册的回调业务函数
    __gnu_cxx::hash_map<int, msg_callback*> _router;
    //回调业务函数对应的参数，key为msgID, value为对应的参数
    __gnu_cxx::hash_map<int, void*> _args;
};
