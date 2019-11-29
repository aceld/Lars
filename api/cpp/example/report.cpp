#include "lars_api.h"
#include <iostream>


void usage()
{
    printf("usage: ./report ModID CmdID IP Port 0|1 --- 0:succ, 1:overload \n");
}

int main(int argc, char **argv)
{
    int ret = 0;

    if (argc != 6) {
        usage();
        return 1;
    }

    int modid = atoi(argv[1]);
    int cmdid = atoi(argv[2]);
    std::string ip = argv[3]; 
    int port = atoi(argv[4]);
    int ret_code = atoi(argv[5]);

    lars_client api;



    //1. lars_api 初始化(只调用一次)
    ret = api.reg_init(modid, cmdid);
    if (ret != 0) {
        std::cout << "modid " << modid << ", cmdid " << cmdid << " still not exist host, after register, ret = " << ret << std::endl;
    }

    api.report(modid, cmdid, ip, port, ret_code);

    std::string result = (ret_code == 0)? "SUCC" :"OVERLOAD";

    std::cout << "report modid = " << modid << ", cmdid = " << cmdid << " | " <<  ip << ":" << port << " " << result << std::endl;

    return 0;
}
