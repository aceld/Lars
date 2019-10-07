#include "lars_reactor.h"
#include "mysql.h"

int main(int argc, char **argv)
{
    event_loop loop;

    //加载配置文件
    config_file::setPath("conf/lars_dns.conf");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 7778);


    //创建tcp服务器
    tcp_server *server = new tcp_server(&loop, ip.c_str(), port);

    //注册路由业务
    
    //测试mysql接口
    MYSQL dbconn;
    mysql_init(&dbconn);

    //开始事件监听    
    printf("lars dns service ....\n");
    loop.event_process();

    return 0;
}
