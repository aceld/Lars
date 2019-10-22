#pragma once
#include "lars_reactor.h"
#include "lars.pb.h"


//与report_client通信的thread_queue消息队列
extern thread_queue<lars::ReportStatusRequest>* report_queue;
//与dns_client通信的thread_queue消息队列
extern thread_queue<lars::GetRouteRequest>* dns_queue;


// 启动udp server服务,用来接收业务层(调用者/使用者)的消息
void start_UDP_servers(void);

// 启动lars_reporter client 线程
void start_report_client(void);

// 启动lars_dns client 线程
void start_dns_client(void);


