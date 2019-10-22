#include "lars_reactor.h"
#include "main_server.h"
#include <pthread.h>

void *dns_client_thread(void* args)
{
    printf("dns client thread start\n");

    return NULL;
}


void start_dns_client()
{
    //开辟一个线程
    pthread_t tid;

    //启动线程业务函数
    int ret = pthread_create(&tid, NULL, dns_client_thread, NULL);
    if (ret == -1) {
        perror("pthread_create");
        exit(1);
    }

    //设置分离模式
    pthread_detach(tid);
}
