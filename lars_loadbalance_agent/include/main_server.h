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

    //当idle节点切换至over_load时的初始化失败次数，主要为了累计一定成功次数才能切换会idle
    int init_err_cnt;

    //当idle节点失败率高于此值，节点变overload状态
    float err_rate;

    //当overload节点成功率高于此值，节点变成idle状态
    float succ_rate;

    //当idle节点连续失败次数超过此值，节点变成overload状态
    int contin_err_limit;

    //当overload节点连续成功次数超过此值, 节点变成idle状态
    int contin_succ_limit;

    //当前agent本地ip地址(用于上报 填充caller字段)
    uint32_t local_ip;

    //整个窗口的真实失败率阈值
    float window_err_rate;

    //对于某个modid/cmdid下的某个idle状态的host，需要清理一次负载信息的周期
    long idle_timeout;

    //对于某个modid/cmdid/下的某个overload状态的host，在过载队列等待的最大时间
    long overload_timeout;

    //对于每个NEW状态的modid/cmdid，多久更新一下本地路由,秒
    long update_timeout;
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


