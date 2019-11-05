#include "lars_reactor.h"
#include "main_server.h"


void * agent_server_main(void * args)
{
    int *index = (int*)args;

    short port = *index + 8888;
    event_loop loop;

    udp_server server(&loop, "0.0.0.0", port);

    //TODO 给server注册消息分发路由业务
    //server.add_msg_router(lars::ID_GetHostRequest, get_host_cb,  NULL);


    printf("agent UDP server :port %d is started...\n", port);
    loop.event_process();

    return NULL;
}


void start_UDP_servers(void)
{
    for (int i = 0; i < 3; i ++) {
        pthread_t tid;
        
        int ret = pthread_create(&tid, NULL, agent_server_main, &i);
        if (ret == -1) {
            perror("pthread_create");
            exit(1);
        }

        pthread_detach(tid);

    }
}
