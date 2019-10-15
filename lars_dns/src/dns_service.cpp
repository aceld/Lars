#include <ext/hash_set>
#include "lars_reactor.h"
#include "subscribe.h"
#include "dns_route.h"
#include "lars.pb.h"

tcp_server *server;

using __gnu_cxx::hash_set;

typedef hash_set<uint64_t> client_sub_mod_list;


//typedef void msg_callback(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data);
void get_route(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    //1. 解析proto文件
    lars::GetRouteRequest req;

    req.ParseFromArray(data, len);

     
    //2. 得到modid 和 cmdid
    int modid, cmdid;

    modid = req.modid();
    cmdid = req.cmdid();

    //2.5 如果之前没有订阅过次modid/cmdid 则订阅
    uint64_t mod = (((uint64_t)modid) << 32) + cmdid;
    client_sub_mod_list *sub_list = (client_sub_mod_list*)conn->param;
    if (sub_list == NULL) {
        printf("sub_list == NULL\n") ;
    }
    if (sub_list->find(mod) == sub_list->end()) {
        //之前没有订阅过该mod,需要订阅
        sub_list->insert(mod);

        //全局订阅列表也要订阅
        SubscribeList::instance()->subscribe(mod, conn->get_fd());
    }
     
    
    //3. 根据modid/cmdid 获取 host信息
    host_set hosts = Route::instance()->get_hosts(modid, cmdid);

    //4. 将数据打包成protobuf
    lars::GetRouteResponse rsp;

    rsp.set_modid(modid);
    rsp.set_cmdid(cmdid);
    
    for (host_set_it it = hosts.begin(); it != hosts.end(); it ++) {
        uint64_t ip_port = *it;
        lars::HostInfo host;
        host.set_ip((uint32_t)(ip_port >> 32));
        host.set_port((int)(ip_port));
        rsp.add_host()->CopyFrom(host);
    }
    
    //5. 发送给客户端
    std::string responseString;
    rsp.SerializeToString(&responseString);
    conn->send_message(responseString.c_str(), responseString.size(), lars::ID_GetRouteResponse);
}

//订阅route 的modid/cmdid
void create_subscribe(net_connection * conn, void *args)
{
    conn->param = new client_sub_mod_list;
}

//退订route 的modid/cmdid
void clear_subscribe(net_connection * conn, void *args)
{
    client_sub_mod_list::iterator it;
    client_sub_mod_list *sub_list = (client_sub_mod_list*)conn->param;

    for (it = sub_list->begin(); it  != sub_list->end(); it++) {
        uint64_t mod = *it;
        SubscribeList::instance()->unsubscribe(mod, conn->get_fd());
    }

    delete sub_list;

    conn->param = NULL;
}

int main(int argc, char **argv)
{
    event_loop loop;

    //加载配置文件
    config_file::setPath("conf/lars_dns.conf");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 7778);


    //创建tcp服务器
    server = new tcp_server(&loop, ip.c_str(), port);

    //注册链接创建/销毁Hook函数
    server->set_conn_start(create_subscribe);
    server->set_conn_close(clear_subscribe);

    //注册路由业务
    server->add_msg_router(lars::ID_GetRouteRequest, get_route);


    //开辟backend thread 周期性检查db数据库route信息的更新状态
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, check_route_changes, NULL);
    if (ret == -1) {
        perror("pthread_create backendThread");
        exit(1);
    }
    //设置分离模式
    pthread_detach(tid);

    //开始事件监听    
    printf("lars dns service ....\n");
    loop.event_process();

    return 0;
}
