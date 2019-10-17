#include "lars_reactor.h"
#include "lars.pb.h"



void report_status(net_connection *conn, void *user_data)
{
    tcp_client *client = (tcp_client*)conn;

    lars::ReportStatusRequest req; 

    //组装测试消息
    req.set_modid(rand() % 3 + 1);
    req.set_cmdid(1);
    req.set_caller(123);
    req.set_ts(time(NULL));

    for (int i = 0; i < 3; i ++) {
        lars::HostCallResult result;
        result.set_ip(i + 1);
        result.set_port((i + 1) * (i + 1));

        result.set_succ(100);
        result.set_err(3);
        result.set_overload(true);
        req.add_results()->CopyFrom(result);
    }


    std::string requestString;
    req.SerializeToString(&requestString);

    //发送给reporter service
    client->send_message(requestString.c_str(), requestString.size(), lars::ID_ReportStatusRequest);
}


void connection_build(net_connection *conn, void *args)
{
    report_status(conn, args);
}


int main(int argc, char **argv)
{
    event_loop loop;

    tcp_client client(&loop, "127.0.0.1", 7779, "reportClient");

    //添加建立连接成功业务
    client.set_conn_start(connection_build);

    loop.event_process();
    
    return 0;
}
