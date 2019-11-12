#pragma once
#include "lars_reactor.h"
#include "lars.pb.h"
#include "route_lb.h"

struct load_balance_config
{
    //经过若干次获取请求host节点后，试探选择一次overload过载节点
    int probe_num; 

    //初始化host_info主机信息访问成功的个数，防止刚启动时少量失败就认为过载
    int init_succ_cnt;
};

//全局配置
extern struct load_balance_config lb_config;

//与report_client通信的thread_queue消息队列
extern thread_queue<lars::ReportStatusRequest>* report_queue;
//与dns_client通信的thread_queue消息队列
extern thread_queue<lars::GetRouteRequest>* dns_queue;

//每个Agent UDP server的 负载均衡器路由 route_lb
extern route_lb * r_lb[3];



// 启动udp server服务,用来接收业务层(调用者/使用者)的消息
void start_UDP_servers(void);

// 启动lars_reporter client 线程
void start_report_client(void);

// 启动lars_dns client 线程
void start_dns_client(void);


