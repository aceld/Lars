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

    int ret = api.get_host(modid, cmdid, ip, port);
    if (ret == 0) {
        std::cout << "host is " << ip << ":" << port << std::endl;
        //上报调用结果
    }

   

    return 0;
}
