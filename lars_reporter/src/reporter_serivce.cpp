#include "lars_reactor.h"
#include "lars.pb.h"
#include "store_report.h"
#include <string>



thread_queue<lars::ReportStatusRequest> **reportQueues = NULL;


void get_report_status(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    lars::ReportStatusRequest req;

    req.ParseFromArray(data, len);

    //将上报数据存储到db 
    StoreReport sr;
    sr.store(req);
}



#if 0
void create_reportdb_threads()
{
    int thread_cnt = config_file::instance()->GetNumber("reporter", "db_thread_cnt", 3);
    
    //开线程池的消息队列
    reportQueues = new thread_queue<lars::ReportStatusRequest>*[thread_cnt];

    if (reportQueues == NULL) {
        fprintf(stderr, "create thread_queue<lars::ReportStatusRequest>*[%d], error", thread_cnt) ;
        exit(1);
    }
}
#endif


int main(int argc, char **argv)
{
    event_loop loop;

    //加载配置文件
    config_file::setPath("./conf/lars_reporter.conf");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 7779);


    //创建tcp server
    tcp_server server(&loop, ip.c_str(), port);

    //添加数据上报请求处理的消息分发处理业务
    server.add_msg_router(lars::ID_ReportStatusRequest, get_report_status);

#if 0
    //为了防止在业务中出现io阻塞，那么需要启动一个线程池对IO进行操作的，接受业务的请求存储消息
    create_reportdb_threads();
#endif
  
    //启动事件监听
    loop.event_process(); 

    return 0;
}



