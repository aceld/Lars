#pragma once

#include "lars_reactor.h"
#include <string>

class lars_client
{
public:
    lars_client();
    ~lars_client();

    //lars 系统获取host信息 得到可用host的ip和port
    int get_host(int modid, int cmdid, std::string& ip, int &port);

private:
    int _sockfd[3]; //3个udp socket fd 对应agent 3个udp server
    uint32_t _seqid; //消息的序列号
};
