#pragma once
#include "load_balance.h"

//key: modid+cmdid    value: load_balance
typedef __gnu_cxx::hash_map<uint64_t, load_balance*> route_map;
typedef __gnu_cxx::hash_map<uint64_t, load_balance*>::iterator route_map_it;


/*
 * 针对多组modid/cmdid ,route_lb是管理多个load_balanace模块的
 * 目前设计有3个，和udp-server的数量一致，每个route_lb分别根据
 * modid/cmdid的值做hash，分管不同的modid/cmdid
 *
 * */
class route_lb {
public:
    //构造初始化
    route_lb(int id);

    //agent获取一个host主机，将返回的主机结果存放在rsp中
    int get_host(int modid, int cmdid, lars::GetHostResponse &rsp);

    //根据Dns Service返回的结果更新自己的route_lb_map
    int update_host(int modid, int cmdid, lars::GetRouteResponse &rsp);

    //agent 上报某主机的获取结果
    void report_host(lars::ReportRequest req);

    //将全部的load_balance都重置为NEW状态
    void reset_lb_status();

private:
    route_map _route_lb_map;  //当前route_lb下的管理的loadbalance
    pthread_mutex_t _mutex; 
    int _id; //当前route_lb的ID编号
};
