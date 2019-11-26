#include "lars_api.h"
#include <iostream>


void usage()
{
    printf("usage: ./example [modid] [cmdid]\n");
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        usage();
        return 1;
    }

    int modid = atoi(argv[1]);
    int cmdid = atoi(argv[2]);
    lars_client api;

    std::string ip; 
    int port;

    route_set route;
    int ret = api.get_route(modid, cmdid, route);
    if (ret == 0) {
        std::cout << "get route succ!" << std::endl;
        for (route_set_it it = route.begin(); it != route.end(); it++) {
            std::cout << "ip = " << (*it).first << ", port = " << (*it).second << std::endl;
        }
    }

    ret = api.get_host(modid, cmdid, ip, port);
    if (ret == 0) {
        std::cout << "host is " << ip << ":" << port << std::endl;

        //上报调用结果
        api.report(modid, cmdid, ip, port, 0);
    }

    return 0;
}
