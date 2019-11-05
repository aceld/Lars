#include "main_server.h"
#include "lars.pb.h"

//--------- 全局资源 ----------
struct load_balance_config lb_config;

//与report_client通信的thread_queue消息队列
thread_queue<lars::ReportStatusRequest>* report_queue = NULL;
//与dns_client通信的thread_queue消息队列
thread_queue<lars::GetRouteRequest>* dns_queue = NULL;

//每个Agent UDP server的 负载均衡器路由 route_lb
route_lb * r_lb[3];



static void create_route_lb()
{
    for (int i = 0; i < 3; i++) {
        int id = i + 1; //route_lb的id从1 开始计数
        r_lb[i]  = new route_lb(id);
        if (r_lb[i] == NULL) {
            fprintf(stderr, "no more space to new route_lb\n");
            exit(1);
        }
    }
}

static void init_lb_agent()
{
    //1. 加载配置文件
    lb_config.probe_num = config_file::instance()->GetNumber("loadbalance", "probe_num", 10);

    //2. 初始化3个route_lb模块
    create_route_lb();
}


int main(int argc, char **argv)
{
    //1 初始化环境
    init_lb_agent();

    //1.5 初始化LoadBalance的负载均衡器
    
    //2 启动udp server服务,用来接收业务层(调用者/使用者)的消息
    start_UDP_servers();
    
    //3 启动lars_reporter client 线程
    report_queue = new thread_queue<lars::ReportStatusRequest>();
    if (report_queue == NULL) {
        fprintf(stderr, "create report queue error!\n");
        exit(1);
    }
    start_report_client();
    
    //4 启动lars_dns client 线程
    dns_queue = new thread_queue<lars::GetRouteRequest>();
    if (dns_queue == NULL) {
        fprintf(stderr, "create dns queue error!\n");
        exit(1);
    }
    start_dns_client();
    

    std::cout <<"done!" <<std::endl;
    while (1) {
        sleep(10);
    }

    return 0;
}
