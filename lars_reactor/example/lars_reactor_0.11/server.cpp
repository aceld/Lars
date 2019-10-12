#include "tcp_server.h"
#include <string>
#include <string.h>
#include "config_file.h"

tcp_server *server;

void print_lars_task(event_loop *loop, void *args)
{
    printf("======= Active Task Func! ========\n");
    listen_fd_set fds;
    loop->get_listen_fds(fds);//不同线程的loop，返回的fds是不同的

    //可以向所有fds触发
    listen_fd_set::iterator it;
    //遍历fds
    for (it = fds.begin(); it != fds.end(); it++) {
        int fd = *it;
        tcp_conn *conn = tcp_server::conns[fd]; //取出fd
        if (conn != NULL) {
            int msgid = 101;
            const char *msg = "Hello I am a Task!";
            conn->send_message(msg, strlen(msg), msgid);
        }
    }
}

//回显业务的回调函数
void callback_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("callback_busi ...\n");
    //直接回显
    conn->send_message(data, len, msgid);
}

//打印信息回调函数
void print_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("recv client: [%s]\n", data);
    printf("msgid: [%d]\n", msgid);
    printf("len: [%d]\n", len);
}


//新客户端创建的回调
void on_client_build(net_connection *conn, void *args)
{
    int msgid = 101;
    const char *msg = "welcome! you online..";

    conn->send_message(msg, strlen(msg), msgid);

    //创建链接成功之后触发任务
    server->thread_poll()->send_task(print_lars_task);
}

//客户端销毁的回调
void on_client_lost(net_connection *conn, void *args)
{
    printf("connection is lost !\n");
}


int main() 
{
    event_loop loop;

    //加载配置文件
    config_file::setPath("./serv.conf");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 8888);

    printf("ip = %s, port = %d\n", ip.c_str(), port);

    server = new tcp_server(&loop, ip.c_str(), port);

    //注册消息业务路由
    server->add_msg_router(1, callback_busi);
    server->add_msg_router(2, print_busi);

    //注册链接hook回调
    server->set_conn_start(on_client_build);
    server->set_conn_close(on_client_lost);


    loop.event_process();

    return 0;
}
