#pragma once
#include <stdint.h>

/*
 *   被代理的主机基本信息
 *
 * */
class host_info {
public:
    host_info(uint32_t ip, int port, uint32_t init_vsucc):
        ip(ip),
        port(port),
        vsucc(init_vsucc),
        verr(0),
        rsucc(0),
        rerr(0),
        contin_succ(0),
        contin_err(0),
        overload(false)
    {

    }

    void set_idle();
    void set_overload();

    //计算整个窗口内的真实失败率，如果达到连续失败窗口值则返回true，代表需要调整状态
    bool check_window();

    uint32_t ip;            //host被代理主机IP
    int port;               //host被代理主机端口
    uint32_t vsucc;         //虚拟成功次数(API反馈)，用于过载(overload)，空闲(idle)判定
    uint32_t verr;          //虚拟失败个数(API反馈)，用于过载(overload)，空闲(idle)判定
    uint32_t rsucc;         //真实成功个数, 给Reporter上报用户观察
    uint32_t rerr;          //真实失败个数，给Reporter上报用户观察
    uint32_t contin_succ;   //连续成功次数
    uint32_t contin_err;    //连续失败次数

    bool overload;          //是否过载

    long idle_ts;           //当前节点成为idle状态的时刻 
    long overload_ts;       //当节点成为overload状态的时刻 
};


