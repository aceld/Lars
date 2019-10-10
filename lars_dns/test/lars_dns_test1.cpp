#include <string.h>
#include <unistd.h>
#include <string>
#include "lars_reactor.h"
#include "lars.pb.h"

//命令行参数
struct Option
{
    Option():ip(NULL),port(0) {}

    char *ip;
    short port;
};


Option option;

void Usage() {
    printf("Usage: ./lars_dns_test -h ip -p port\n");
}

//解析命令行
void parse_option(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            option.ip = argv[i + 1];
        }
        else if (strcmp(argv[i], "-p") == 0) {
            option.port = atoi(argv[i + 1]);
        }
    }

    if ( !option.ip || !option.port ) {
        Usage();
        exit(1);
    }

}

//typedef void (*conn_callback)(net_connection *conn, void *args);
void on_connection(net_connection *conn, void *args) 
{
    //发送Route信息请求
    lars::GetRouteRequest req;

    req.set_modid(1);
    req.set_cmdid(2);

    std::string requestString;

    req.SerializeToString(&requestString);
    conn->send_message(requestString.c_str(), requestString.size(), lars::ID_GetRouteRequest);
}



void deal_get_route(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data)
{
    //解包得到数据
    lars::GetRouteResponse rsp;
    rsp.ParseFromArray(data, len);
    
    //打印数据
    printf("modid = %d\n", rsp.modid());
    printf("cmdid = %d\n", rsp.cmdid());
    printf("host_size = %d\n", rsp.host_size());

    for (int i = 0; i < rsp.host_size(); i++) {
        printf("-->ip = %u\n", rsp.host(i).ip());
        printf("-->port = %d\n", rsp.host(i).port());
    }
    
    //再请求
    lars::GetRouteRequest req;
    req.set_modid(rsp.modid());
    req.set_cmdid(rsp.cmdid());
    std::string requestString;
    req.SerializeToString(&requestString);
    net_conn->send_message(requestString.c_str(), requestString.size(), lars::ID_GetRouteRequest);
}


int main(int argc, char **argv)
{
    parse_option(argc, argv);

    event_loop loop;
    tcp_client *client;

    //创建客户端
    client = new tcp_client(&loop, option.ip, option.port, "lars_dns_test");
    if (client == NULL) {
        fprintf(stderr, "client == NULL\n");
        exit(1);
    }

    //客户端成功建立连接，首先发送请求包
    client->set_conn_start(on_connection);

    //设置服务端回应包处理业务
    client->add_msg_router(lars::ID_GetRouteResponse, deal_get_route);
    

    loop.event_process(); 

    return 0;
}
