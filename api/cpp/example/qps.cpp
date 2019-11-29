#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include "lars_api.h"

struct ID
{
    int t_id;
    int modid;
    int cmdid;
};


void *test_qps(void *args)
{
    int ret = 0;

    ID *id = (ID*)args;

    int modid = id->modid;
    int cmdid = id->cmdid;

    lars_client api;

    std::string ip;
    int port;

    //qps记录
    long qps = 0;

    //记录最后时间
    long last_time = time(NULL);

    long total_qps = 0;
    long total_time_second = 0;

    //1. lars_api 初始化(只调用一次)
    ret = api.reg_init(modid, cmdid);
    if (ret != 0) {
        std::cout << "modid " << modid << ", cmdid " << cmdid << " still not exist host, after register, ret = " << ret << std::endl;
    }
    
    while (1) {
        ret = api.get_host(modid, cmdid, ip, port);
        if (ret == 0 || ret == 1 || ret == 3) { // 成功,过载，不存在 均是合法返回
            ++qps;
            if (ret == 0) {
                api.report(modid, cmdid, ip, port, 0);//上报成功
            }
            else if (ret == 1) {
                api.report(modid, cmdid, ip, port, 1);//上报过载
            }
        }
        else {
            printf("[%d,%d] get error %d\n", modid, cmdid, ret);
        }

        //当前时间
        long current_time = time(NULL);

        if (current_time - last_time >= 1) {
            total_time_second += 1;
            total_qps += qps;
            last_time = current_time;
            printf("thread:[%d] --> qps = [%ld], average = [%ld]\n", id->t_id, qps, total_qps/total_time_second);
            qps = 0;
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("./qps [thread_num]\n");
        return 1;
    }

    int cnt = atoi(argv[1]);

    ID *ids = new ID[cnt];
    pthread_t *tids = new pthread_t[cnt];

    //制作模拟的modid/cmdid
    for (int i = 0; i < cnt; i++) {
        ids[i].t_id = i;
        ids[i].modid = i + 1;
        ids[i].cmdid = 1;
    }

    for (int i = 0; i < cnt; i++) {
        pthread_create(&tids[i], NULL, test_qps, &ids[i]);
    }

    for (int i = 0; i < cnt; i++) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}
