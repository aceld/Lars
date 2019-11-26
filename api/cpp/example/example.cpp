#include "lars_api.h"
#include <iostream>


void usage()
{
    printf("usage: ./example [modid] [cmdid]\n");
}

int main(int argc, char **argv)
{
    int ret = 0;

    if (argc != 3) {
        usage();
        return 1;
    }

    int modid = atoi(argv[1]);
    int cmdid = atoi(argv[2]);
    lars_client api;

    std::string ip; 
    int port;


    //1. lars_api 初始化(只调用一次)
    ret = api.reg_init(modid, cmdid);
    if (ret != 0) {
        std::cout << "modid " << modid << ", cmdid " << cmdid << " still not exist host, after register, ret = " << ret << std::endl;
    }

    //2. 获取modid/cmdid下全部的host的ip+port
    route_set route;
    ret = api.get_route(modid, cmdid, route);
    if (ret == 0) {
        std::cout << "get route succ!" << std::endl;
        for (route_set_it it = route.begin(); it != route.end(); it++) {
            std::cout << "ip = " << (*it).first << ", port = " << (*it).second << std::endl;
        }
    }

    //3. 获取一个host的ip+port
    int cnt = 0;
    ret = api.get_host(modid, cmdid, ip, port);
    if (ret == 0) {
        std::cout << "host is " << ip << ":" << port << std::endl;

        //上报调用结果 0 表示成功， 1 表示过载
        //这里为了测试结果，随机添加过载记录
        
        if (cnt % 3 == 0) {
            api.report(modid, cmdid, ip, port, 1);
        }
        else {
            api.report(modid, cmdid, ip, port, 0);
        }
    }

    return 0;
}
