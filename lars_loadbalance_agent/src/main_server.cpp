#include "main_server.h"
#include "lars.pb.h"

//与report_client通信的thread_queue消息队列
thread_queue<lars::ReportStatusRequest>* report_queue = NULL;
//与dns_client通信的thread_queue消息队列
thread_queue<lars::GetRouteRequest>* dns_queue = NULL;

int main(int argc, char **argv)
{
    //1 加载配置文件
    
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
