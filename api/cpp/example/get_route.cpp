#include "lars_api.h"
#include <iostream>


void usage()
{
    printf("usage: ./get_route [modid] [cmdid]\n");
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

    //1. lars_api 初始化(只调用一次)
    ret = api.reg_init(modid, cmdid);
    if (ret != 0) {
        std::cout << "modid " << modid << ", cmdid " << cmdid << " still not exist host, after register, ret = " << ret << std::endl;
    }

    //2. 获取modid/cmdid下全部的host的ip+port
    route_set route;
    ret = api.get_route(modid, cmdid, route);
    if (ret == 0) {
        for (route_set_it it = route.begin(); it != route.end(); it++) {
            std::cout << "ip = " << (*it).first << ", port = " << (*it).second << std::endl;
        }
    }

    return 0;
}
