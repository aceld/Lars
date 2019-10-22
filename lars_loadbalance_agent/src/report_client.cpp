#include "lars_reactor.h"
#include "main_server.h"
#include <pthread.h>



void *report_client_thread(void* args)
{
    printf("report client thread start\n");
#if 0
    event_loop loop;

    //1 加载配置文件得到repoter ip + port
    std::string ip = config_file::instance()->GetString("reporter", "ip", "");
    short port = config_file::instance()->GetNumber("reporter", "port", 0);

    //2 创建客户端
    tcp_client client(&loop, ip.c_str(), port, "reporter client");

    //3 将 thread_queue消息回调事件，绑定到loop中
    report_queue->set_loop(&loop);
    report_queue->set_callback()



    //4 启动事件监听
    loop.event_process();
#endif

    return NULL;
}


void start_report_client()
{
    //开辟一个线程
    pthread_t tid;

    //启动线程业务函数
    int ret = pthread_create(&tid, NULL, report_client_thread, NULL);
    if (ret == -1) {
        perror("pthread_create");
        exit(1);
    }

    //设置分离模式
    pthread_detach(tid);
}
