#include "lars_api.h"
#include "lars.pb.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

lars_client::lars_client():_seqid(0)
{
 //   printf("lars_client()\n");
    //1 初始化服务器地址
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //默认的ip地址是本地，因为是API和agent应该部署于同一台主机上
    inet_aton("127.0.0.1", &servaddr.sin_addr);

    //2. 创建3个UDPsocket
    for (int i = 0; i < 3; i ++) {
        _sockfd[i] = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
        if (_sockfd[i] == -1) {
            perror("socket()");
            exit(1);
        }

        //agent的3个udp端口默认为8888，8889, 8890
        servaddr.sin_port = htons(8888 + i);
        int ret = connect(_sockfd[i], (const struct sockaddr *)&servaddr, sizeof(servaddr));
        if (ret == -1) {
            perror("connect()");
            exit(1);
        }
//        printf("connection agent udp server succ!\n");
    }
}


lars_client::~lars_client()
{
//    printf("~lars_client()\n");

    for (int i = 0; i < 3; ++i) {
        close(_sockfd[i]);
    }
}



//lars 系统初始化注册modid/cmdid使用(首次拉取)(初始化使用，只调用一次即可)
int lars_client::reg_init(int modid, int cmdid)
{
    route_set route;

    int retry_cnt = 0;

    while (route.empty() && retry_cnt < 3) {
        get_route(modid, cmdid, route);
        if (route.empty() == true) {
            usleep(50000); // 等待50ms
        }
        else {
            break;
        }
        ++retry_cnt;//尝试3次
    }

    if (route.empty() == true) {
        return lars::RET_NOEXIST;//3
    }

    return lars::RET_SUCC; //0
}

//lars 系统获取某modid/cmdid全部的hosts(route)信息
int lars_client::get_route(int modid, int cmdid, route_set &route)
{
    //1. 封装请求消息
    lars::GetRouteRequest req;
    req.set_modid(modid);
    req.set_cmdid(cmdid);

    //2. send
    char write_buf[4096], read_buf[80*1024];
    //消息头
    msg_head head;
    head.msglen = req.ByteSizeLong();
    head.msgid = lars::ID_API_GetRouteRequest;
    memcpy(write_buf, &head, MESSAGE_HEAD_LEN);
    
    //消息体
    req.SerializeToArray(write_buf+MESSAGE_HEAD_LEN, head.msglen);

    //简单的hash来发给对应的agent udp server
    int index = (modid + cmdid) %3;
    int ret = sendto(_sockfd[index], write_buf, head.msglen + MESSAGE_HEAD_LEN, 0, NULL, 0);
    if (ret == -1) {
        perror("sendto");
        return lars::RET_SYSTEM_ERROR;
    }
    
    //3. recv
    lars::GetRouteResponse rsp;

    int message_len = recvfrom(_sockfd[index], read_buf, sizeof(read_buf), 0, NULL, NULL);
    if (message_len == -1) {
        perror("recvfrom");
        return lars::RET_SYSTEM_ERROR;
    }

    //消息头
    memcpy(&head, read_buf, MESSAGE_HEAD_LEN);
    if (head.msgid != lars::ID_API_GetRouteResponse) {
        fprintf(stderr, "message ID error!\n");
        return lars::RET_SYSTEM_ERROR;
    }

    //消息体 
    ret = rsp.ParseFromArray(read_buf + MESSAGE_HEAD_LEN, message_len - MESSAGE_HEAD_LEN);
    if (!ret) {
        fprintf(stderr, "message format error: head.msglen = %d, message_len = %d, message_len - MESSAGE_HEAD_LEN = %d, head msgid = %d, ID_GetHostResponse = %d\n", head.msglen, message_len, message_len-MESSAGE_HEAD_LEN, head.msgid, lars::ID_GetRouteResponse);
        return lars::RET_SYSTEM_ERROR;
    }

    if (rsp.modid() != modid || rsp.cmdid() != cmdid) {
        fprintf(stderr, "message format error\n");
        return lars::RET_SYSTEM_ERROR;
    }

    //4 处理消息
    for (int i = 0; i < rsp.host_size(); i++) {
        const lars::HostInfo &host = rsp.host(i);
        struct in_addr inaddr;
        inaddr.s_addr = host.ip();
        std::string ip = inet_ntoa(inaddr);
        int port = host.port();
        route.push_back(ip_port(ip,port));
    }

    return lars::RET_SUCC;
}


//lars 系统获取host信息 得到可用host的ip和port
int lars_client::get_host(int modid, int cmdid, std::string &ip, int &port)
{
    //从本地agent service获取 host信息  
    uint32_t seq = _seqid++;  
    
    //1. 封装请求信息
    lars::GetHostRequest req;
    req.set_seq(seq);//序列编号
    req.set_modid(modid);
    req.set_cmdid(cmdid);
    
    //2. send
    char write_buf[4096], read_buf[80*1024];
    //消息头
    msg_head head;
    head.msglen = req.ByteSizeLong();
    head.msgid = lars::ID_GetHostRequest;
    memcpy(write_buf, &head, MESSAGE_HEAD_LEN);
    
    //消息体
    req.SerializeToArray(write_buf+MESSAGE_HEAD_LEN, head.msglen);

    //简单的hash来发给对应的agent udp server
    int index = (modid + cmdid) %3;
    int ret = sendto(_sockfd[index], write_buf, head.msglen + MESSAGE_HEAD_LEN, 0, NULL, 0);
    
    if (ret == -1) {
        perror("sendto");
        return lars::RET_SYSTEM_ERROR;
    }
    
    //3. recv
    int message_len;
    lars::GetHostResponse rsp;
    do {
        message_len = recvfrom(_sockfd[index], read_buf, sizeof(read_buf), 0, NULL, NULL);
        if (message_len == -1) {
            perror("recvfrom");
            return lars::RET_SYSTEM_ERROR;
        }

        //消息头
        memcpy(&head, read_buf, MESSAGE_HEAD_LEN);
        if (head.msgid != lars::ID_GetHostResponse) {
            fprintf(stderr, "message ID error!\n");
            return lars::RET_SYSTEM_ERROR;
        }

        //消息体 
        ret = rsp.ParseFromArray(read_buf + MESSAGE_HEAD_LEN, message_len - MESSAGE_HEAD_LEN);
        if (!ret) {
            fprintf(stderr, "message format error: head.msglen = %d, message_len = %d, message_len - MESSAGE_HEAD_LEN = %d, head msgid = %d, ID_GetHostResponse = %d\n", head.msglen, message_len, message_len-MESSAGE_HEAD_LEN, head.msgid, lars::ID_GetHostResponse);
            return lars::RET_SYSTEM_ERROR;
        }
    } while (rsp.seq() < seq);


    if (rsp.seq() != seq || rsp.modid() != modid || rsp.cmdid() != cmdid) {
        fprintf(stderr, "message format error\n");
        return lars::RET_SYSTEM_ERROR;
    }

    //4 处理消息
    if (rsp.retcode() == 0) {
        lars::HostInfo host = rsp.host();

        struct in_addr inaddr;
        inaddr.s_addr = host.ip();
        ip = inet_ntoa(inaddr);
        port = host.port();
    }

    return rsp.retcode();//lars::RET_SUCC
}



//lars 系统上报host调用信息
void lars_client::report(int modid, int cmdid, const std::string &ip, int port, int retcode)
{
    //1 封装上报消息
    lars::ReportRequest req;    

    req.set_modid(modid);
    req.set_cmdid(cmdid);
    req.set_retcode(retcode);

    //1.1 host信息
    lars::HostInfo *hp = req.mutable_host();

    //ip
    struct in_addr inaddr;
    inet_aton(ip.c_str(), &inaddr);
    int ip_num = inaddr.s_addr;
    hp->set_ip(ip_num);
    
    //port
    hp->set_port(port);

    //2. send
    char write_buf[4096];
    //消息头
    msg_head head;
    head.msglen = req.ByteSizeLong();
    head.msgid = lars::ID_ReportRequest;
    memcpy(write_buf, &head, MESSAGE_HEAD_LEN);
    req.SerializeToArray(write_buf + MESSAGE_HEAD_LEN, head.msglen);

    int index = (modid+cmdid)%3;
    int ret = sendto(_sockfd[index], write_buf, head.msglen + MESSAGE_HEAD_LEN, 0, NULL, 0);
    if (ret == -1) {
        perror("sendto");
    }
}


