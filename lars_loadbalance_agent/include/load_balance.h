#pragma once
#include <ext/hash_map>
#include <list>
#include "host_info.h"
#include "lars.pb.h"

//ip + port为主键的 host信息集合
typedef __gnu_cxx::hash_map<uint64_t, host_info*>   host_map;   // key:uint64(ip+port), value:host_info
typedef __gnu_cxx::hash_map<uint64_t, host_info*>::iterator host_map_it;

//host_info list集合
typedef std::list<host_info*> host_list; 
typedef std::list<host_info*>::iterator host_list_it;


/*
 * 负载均衡算法核心模块
 * 针对一组(modid/cmdid)下的全部host节点的负载规则
 */
class load_balance {
public:
    load_balance(int modid, int cmdid):
        status(PULLING),
        _modid(modid),
        _cmdid(cmdid)
    {
        //load_balance 初始化构造
    }


    //判断是否已经没有host在当前LB节点中
    bool empty() const;

    //从当前的双队列中获取host信息
    int choice_one_host(lars::GetHostResponse &rsp);

    //如果list中没有host信息，需要从远程的DNS Service发送GetRouteHost请求申请
    int pull();

    //根据dns service远程返回的结果，更新_host_map
    void update(lars::GetRouteResponse &rsp);

    //上报当前host主机调用情况给远端repoter service
    void report(int ip, int port, int retcode);

    //提交host的调用结果给远程reporter service上报结果
    void commit();


    //当前load_balance模块的状态
    enum STATUS
    {
        PULLING, //正在从远程dns service通过网络拉取
        NEW      //正在创建新的load_balance模块
    };
    STATUS status;  //当前的状态
    
private:

    int _modid;
    int _cmdid;
    int _access_cnt;    //请求次数，每次请求+1,判断是否超过probe_num阈值

    host_map _host_map; //当前load_balance模块所管理的全部ip + port为主键的 host信息集合

    host_list _idle_list;       //空闲队列
    host_list _overload_list;   //过载队列
};
