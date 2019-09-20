#include "tcp_client.h"
#include <stdio.h>
#include <string.h>


//客户端业务
void busi(const char *data, uint32_t len, int msgid, tcp_client *conn, void *user_data)
{
    //得到服务端回执的数据 
    
    printf("recv server: [%s]\n", data);
    printf("msgid: [%d]\n", msgid);
    printf("len: [%d]\n", len);
}


int main() 
{

    event_loop loop;

    //创建tcp客户端
    tcp_client client(&loop, "127.0.0.1", 7777, "clientv0.4");


    //注册回调业务
    client.set_msg_callback(busi);

    //开启事件监听
    loop.event_process();

    return 0;
}



