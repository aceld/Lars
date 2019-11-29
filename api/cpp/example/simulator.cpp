#include "lars_api.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <map>

void usage()
{
    printf("usage: ./simulator ModID CmdID [errRate(0-10)] [query cnt(0-999999)]\n");
}

int main(int argc, char **argv)
{
    int ret = 0;

    if (argc < 3) {
        usage();
        return 1;
    }

    int modid = atoi(argv[1]);
    int cmdid = atoi(argv[2]);
    int err_rate = 2;
    int query_cnt = 100;

    if (argc > 3) {
        err_rate = atoi(argv[3]);
    }

    if (argc > 4) {
        query_cnt = atoi(argv[4]);
    }

    lars_client api;

    std::string ip;
    int port;
    //key---ip,  value---<succ_cnt, err_cnt>
    std::map<std::string, std::pair<int, int>> result;

    std::cout << "err_rate = " << err_rate << std::endl;

    //1. lars_api 初始化(只调用一次)
    ret = api.reg_init(modid, cmdid);
    if (ret != 0) {
        std::cout << "modid " << modid << ", cmdid " << cmdid << " still not exist host, after register, ret = " << ret << std::endl;
    }

    srand(time(NULL));

    for (int i = 0; i < query_cnt; i++) {
        ret = api.get_host(modid, cmdid, ip, port);
        if (ret == 0) {
            //获取成功
            if (result.find(ip) == result.end()) {
                // 首次获取当前ip
                std::pair<int ,int> succ_err(0, 0);
                result[ip] = succ_err;
            }

            std::cout << "host " << ip << ":" << "host" << "called ";
            if (rand()%10 < err_rate) {// 80%的几率产生调用失败
                result[ip].second += 1;
                api.report(modid, cmdid, ip, port, 1);
                std::cout << " ERROR!!!" << std::endl;
            }
            else {
                result[ip].first += 1;
                api.report(modid, cmdid, ip, port, 0);
                std::cout << " SUCCESS." << std::endl;
            }
        }
        else if (ret == 3) {
            std::cout << modid << "," << cmdid << " not exist" << std::endl;
        }
        else if (ret == 2) {
            std::cout << "system err" << std::endl;
        }
        else if (ret == 1) {
            std::cout << modid << "," << cmdid << " all hosts were overloaded!!!" << std::endl;
        }
        else {
            std::cout << "get error code " << ret << std::endl;
        }
        usleep(6000);
    }

    
    //遍历结果
    std::map<std::string, std::pair<int, int>>::iterator it;
    for (it = result.begin(); it != result.end(); it ++) {
        std::cout <<"ip : " << it->first << ": ";
        std::cout <<"success: " << it->second.first << "; ";
        std::cout <<"error: " << it->second.second << std::endl;
    }
    

    return 0;
}
