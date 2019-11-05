#include "main_server.h"
#include "load_balance.h"
#include "lars.pb.h"


//判断是否已经没有host在当前LB节点中
bool load_balance::empty() const
{
    return _host_map.empty();
}


//从一个host_list中得到一个节点放到GetHostResponse 的HostInfo中
static void get_host_from_list(lars::GetHostResponse &rsp, host_list &l)
{
    //选择list中第一个节点
    host_info *host = l.front();

    //HostInfo自定义类型,proto3并没提供set方法,而是通过mutable_接口返回HostInfo的指针,可根据此指针进行赋值操作
    lars::HostInfo* hip = rsp.mutable_host();

    hip->set_ip(host->ip);
    hip->set_port(host->port);

    //将上面处理过的第一个节点放在队列的末尾
    l.pop_front();
    l.push_back(host);
}

//从两个队列中获取一个host给到上层
int load_balance::choice_one_host(lars::GetHostResponse &rsp)
{
    //1 判断_dile_list队列是否已经空,如果空表示没有空闲节点 
    if (_idle_list.empty() == true) {

        // 1.1 判断是否已经超过了probe_num
        if (_access_cnt >= lb_config.probe_num) {
            _access_cnt = 0;

            //从 overload_list中选择一个已经过载的节点
            get_host_from_list(rsp, _overload_list);
        }
        else {
            //明确返回给API层，已经过载了
            ++_access_cnt;
            return lars::RET_OVERLOAD;                
        }
    }
    else {
        // 2 判断过载列表是否为空
        if (_overload_list.empty() == true) {
            //2.1 当前modid/cmdid所有节点均为正常
            
            //选择一个idle节点
            get_host_from_list(rsp, _idle_list);
        }
        else {
            //2.2 有部分节点过载
            
            //判断访问次数是否超过probe_num阈值,超过则从overload_list取出一个
            if (_access_cnt >= lb_config.probe_num) {
                _access_cnt = 0;
                get_host_from_list(rsp, _overload_list);
            }
            else {
                //正常从idle_list中选出一个节点
                ++_access_cnt;
                get_host_from_list(rsp, _idle_list);
            }

        
            //选择一个idle节点
            get_host_from_list(rsp, _idle_list);
        }
    }

    return lars::RET_SUCC;
}


//如果list中没有host信息，需要从远程的DNS Service发送GetRouteHost请求申请
int load_balance::pull()
{
    //请求dns service请求
    lars::GetRouteRequest route_req;
    route_req.set_modid(_modid);
    route_req.set_cmdid(_cmdid);
    
    //通过dns client的thread queue发送请求
    dns_queue->send(route_req);

    //由于远程会有一定延迟，所以应该给当前的load_balance模块标记一个正在拉取的状态
    status = PULLING;  

    return 0;
}



//根据dns service远程返回的结果，更新_host_map
int load_balance::update(lars::GetRouteResponse &rsp)
{

    return 0;
}
