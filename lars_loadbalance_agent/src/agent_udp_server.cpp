#include "lars_reactor.h"
#include "main_server.h"



static void get_host_cb(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data)
{
    printf("get_host_cb  is called ....\n");

    //解析api发送的请求包
    lars::GetHostRequest req;         

    req.ParseFromArray(data, len);

    int modid = req.modid();
    int cmdid = req.cmdid();

    //设置回执消息
    lars::GetHostResponse rsp;
    rsp.set_seq(req.seq());
    rsp.set_modid(modid);
    rsp.set_cmdid(cmdid);

    route_lb *ptr_route_lb = (route_lb*)user_data;
    
    //调用route_lb的获取host方法，得到rsp返回结果
    ptr_route_lb->get_host(modid, cmdid, rsp);

    //打包回执给api消息
    std::string responseString; 
    rsp.SerializeToString(&responseString);
    net_conn->send_message(responseString.c_str(), responseString.size(), lars::ID_GetHostResponse);
}

static void report_cb(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data)
{
    lars::ReportRequest req;
    
    req.ParseFromArray(data, len);

    route_lb *ptr_route_lb = (route_lb*)user_data;
    ptr_route_lb->report_host(req);
}


void * agent_server_main(void * args)
{
    long index = (long)args;

    short port = index + 8888;
    event_loop loop;

    udp_server server(&loop, "0.0.0.0", port);

    //给server注册消息分发路由业务, 针对ID_GetHostRequest处理 每个udp拥有一个对应的route_lb
    server.add_msg_router(lars::ID_GetHostRequest, get_host_cb,  r_lb[port-8888]);

    //给server注册消息分发路由业务，针对ID_ReportRequest处理
    server.add_msg_router(lars::ID_ReportRequest, report_cb, r_lb[port-8888]);


    printf("agent UDP server :port %d is started...\n", port);
    loop.event_process();

    return NULL;
}


void start_UDP_servers(void)
{
    for (long i = 0; i < 3; i ++) {
        pthread_t tid;
        
        int ret = pthread_create(&tid, NULL, agent_server_main, (void*)i);
        if (ret == -1) {
            perror("pthread_create");
            exit(1);
        }

        pthread_detach(tid);
    }
}
