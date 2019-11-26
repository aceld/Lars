#pragma once

#include "lars_reactor.h"
#include <string>

typedef std::pair<std::string, int> ip_port;
typedef std::vector<ip_port> route_set;
typedef route_set::iterator route_set_it;

class lars_client
{
public:
    lars_client();
    ~lars_client();

    //lars 系统初始化注册modid/cmdid使用(首次拉取)(初始化使用，只调用一次即可)
    int reg_init(int modid, int cmdid);

    //lars 系统获取host信息 得到可用host的ip和port
    int get_host(int modid, int cmdid, std::string& ip, int &port);

    //lars 系统上报host调用信息
    void report(int modid, int cmdid, const std::string &ip, int port, int retcode);

    //lars 系统获取某modid/cmdid全部的hosts(route)信息
    int get_route(int modid, int cmdid, route_set &route);

private:
    int _sockfd[3]; //3个udp socket fd 对应agent 3个udp server
    uint32_t _seqid; //消息的序列号
};
