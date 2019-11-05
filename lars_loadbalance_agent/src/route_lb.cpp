#include "route_lb.h"
#include "lars.pb.h"


//agent获取一个host主机，将返回的主机结果存放在rsp中
int route_lb::get_host(int modid, int cmdid, lars::GetHostResponse &rsp)
{
    int ret = lars::RET_SUCC;

    //1. 得到key
    uint64_t key = ((uint64_t)modid << 32) + cmdid;

    pthread_mutex_lock(&_mutex);
    //2. 当前key已经存在_route_lb_map中
    if (_route_lb_map.find(key) != _route_lb_map.end()) {
        //2.1 取出对应的load_balance
        load_balance *lb = _route_lb_map[key];

        if (lb->empty() == true) {
            //存在lb 里面的host为空，说明正在pull()中，还没有从dns_service返回来,所以直接回复不存在
            assert(lb->status == load_balance::PULLING);
            rsp.set_retcode(lars::RET_NOEXIST);
        }
        else {
            ret = lb->choice_one_host(rsp);
            rsp.set_retcode(ret);

            //TODO 超时重拉路由
        }

    }
    //3. 当前key不存在_route_lb_map中
    else {
        //3.1 新建一个load_balance
        load_balance *lb = new load_balance(modid, cmdid);
        if (lb == NULL) {
            fprintf(stderr, "no more space to create loadbalance\n");
            exit(1);
        }

        //3.2 新建的load_balance加入到map中
        _route_lb_map[key] = lb;

        //3.3 从dns service服务拉取具体的host信息
        lb->pull();

        //3.4 设置rsp的回执retcode
        rsp.set_retcode(lars::RET_NOEXIST);

        ret = lars::RET_NOEXIST;
    }

    pthread_mutex_unlock(&_mutex);

    return ret;
}

//根据Dns Service返回的结果更新自己的route_lb_map
int route_lb::update_host(int modid, int cmdid, lars::GetRouteResponse &rsp)
{
    //1. 得到key
    uint64_t key = ((uint64_t)modid << 32) + cmdid;

    pthread_mutex_lock(&_mutex);

    //2. 在_route_map中找到对应的key 
    if (_route_lb_map.find(key) != _route_lb_map.end()) {
        
        load_balance *lb = _route_lb_map[key];

        if (rsp.host_size() == 0) {
            //2.1 如果返回的结果 lb下已经没有任何host信息，则删除该key
            delete lb;
            _route_lb_map.erase(key);
        }
        else {
            //2.2 更新新host信息
            lb->update(rsp);
        }
    }
    
    pthread_mutex_unlock(&_mutex);

    return 0;
}
