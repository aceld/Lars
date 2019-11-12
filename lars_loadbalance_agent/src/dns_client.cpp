#include "lars_reactor.h"
#include "main_server.h"
#include <pthread.h>


//typedef void io_callback(event_loop *loop, int fd, void *args);
//只要thread_queue有数据，loop就会触发此回调函数来处理业务
void new_dns_request(event_loop *loop, int fd, void *args)
{
    tcp_client *client = (tcp_client*)args;

    //1. 将请求数据从thread_queue中取出，
    std::queue<lars::GetRouteRequest>  msgs;

    //2. 将数据放在queue队列中
    dns_queue->recv(msgs);
    
    //3. 遍历队列，通过client依次将每个msg发送给reporter service
    while (!msgs.empty()) {
        lars::GetRouteRequest req = msgs.front();
        msgs.pop();

        std::string requestString;
        req.SerializeToString(&requestString);

        //client 发送数据
        client->send_message(requestString.c_str(), requestString.size(), lars::ID_GetRouteRequest);
    }

}


/*
 * 处理远程dns service回复的modid/cmdid对应的路由信息
 * */
void deal_recv_route(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data)
{
    lars::GetRouteResponse rsp;

    //解析远程消息到proto结构体中
    rsp.ParseFromArray(data, len);

    int modid = rsp.modid();
    int cmdid = rsp.cmdid();
    int index = (modid+cmdid)%3;

    // 将该modid/cmdid交给一个route_lb处理 将rsp中的hostinfo集合加入到对应的route_lb中
    r_lb[index]->update_host(modid, cmdid, rsp);
}

void *dns_client_thread(void* args)
{
    printf("dns client thread start\n");
    event_loop loop;

    //1 加载配置文件得到dns service ip + port
    std::string ip = config_file::instance()->GetString("dnsserver", "ip", "");
    short port = config_file::instance()->GetNumber("dnsserver", "port", 0);

    //2 创建客户端
    tcp_client client(&loop, ip.c_str(), port, "dns client");


    //3 将thread_queue消息回调事件，绑定到loop中
    dns_queue->set_loop(&loop);
    dns_queue->set_callback(new_dns_request, &client);

    //4 设置当收到dns service回执的消息ID_GetRouteResponse处理函数
    client.add_msg_router(lars::ID_GetRouteResponse, deal_recv_route);

    //启动事件监听
    loop.event_process(); 

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
