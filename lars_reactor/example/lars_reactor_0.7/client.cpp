#include "tcp_client.h"
#include <stdio.h>
#include <string.h>


//客户端业务
void busi(const char *data, uint32_t len, int msgid, net_connection  *conn, void *user_data)
{
    //得到服务端回执的数据 
    
    printf("recv server: [%s]\n", data);
    printf("msgid: [%d]\n", msgid);
    printf("len: [%d]\n", len);
}


//客户端销毁的回调
void on_client_build(net_connection *conn, void *args)
{
    int msgid = 1; 
    const char *msg = "Hello Lars!";

    conn->send_message(msg, strlen(msg), msgid);
}

//客户端销毁的回调
void on_client_lost(net_connection *conn, void *args) 
{
    printf("on_client_lost...\n");
    printf("Client is lost!\n");
}



int main() 
{

    event_loop loop;

    //创建tcp客户端
    tcp_client client(&loop, "127.0.0.1", 7777, "clientv0.6");


    //注册消息路由业务
    client.add_msg_router(1, busi);
    client.add_msg_router(101, busi);


    //设置hook函数
    client.set_conn_start(on_client_build);
    client.set_conn_close(on_client_lost);


    //开启事件监听
    loop.event_process();

    return 0;
}



