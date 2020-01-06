#pragma once

#include <vector>
#include <pthread.h>
#include <ext/hash_set>
#include <ext/hash_map>
#include "lars_reactor.h"
#include "lars.pb.h"
#include "dns_route.h"

using namespace __gnu_cxx;


//定义订阅列表数据关系类型，key->modid/cmdid， value->fds(订阅的客户端文件描述符)
typedef hash_map<uint64_t, hash_set<int>> subscribe_map;

//定义发布列表的数据关系类型, key->fd(订阅客户端的文件描述符), value->modids
typedef hash_map<int, hash_set<uint64_t>> publish_map;

class SubscribeList {
public:
    //设计单例
    static void init()  {
        _instance = new SubscribeList();
    }

    static SubscribeList *instance() {
        //保证init方法在这个进程执行中，只执行一次
        pthread_once(&_once, init);
        return _instance;
    }

    //订阅
    void subscribe(uint64_t mod, int fd);
    
    //取消订阅
    void unsubscribe(uint64_t mod, int fd);
    
    //发布
    void publish(std::vector<uint64_t> &change_mods);

    //根据在线用户fd得到需要发布的列表
    void make_publish_map(listen_fd_set &online_fds, 
                          publish_map &need_publish);

    publish_map * get_push_list() {
        return &_push_list;
    }
    
    
private:
    //设计单例
    SubscribeList();
    SubscribeList(const SubscribeList &);
    const SubscribeList& operator=(const SubscribeList);

    static SubscribeList *_instance;
    static pthread_once_t _once;


    subscribe_map _book_list; //订阅清单
    pthread_mutex_t _book_list_lock;

    publish_map _push_list; //发布清单
    pthread_mutex_t _push_list_lock;
};
