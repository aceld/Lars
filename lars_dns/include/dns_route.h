#pragma once

#include <pthread.h>
#include <ext/hash_map>
#include <ext/hash_set>
#include "mysql.h"

using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;

//定义用来保存modID/cmdID与host的IP/host的port的对应的关系 数据类型  
typedef hash_map< uint64_t, hash_set<uint64_t> > route_map;
typedef hash_map< uint64_t, hash_set<uint64_t> >::iterator route_map_it;

//定义用来保存host的IP/host的port的的集合 数据类型
typedef hash_set<uint64_t> host_set;
typedef hash_set<uint64_t>::iterator host_set_it;

class Route
{
public:
    //创建单例的方法
    static void init() {
        _instance = new Route(); 
    }

    static Route *instance() {
        //保证init方法在这个进程执行中，只执行一次
        pthread_once(&_once, init);
        return _instance;
    }

    //获取modid/cmdid对应的host信息
    host_set get_hosts(int modid, int cmdid);


    //链接数据库
    void connect_db();

    //构建map
    void build_maps();
    
private:
    //构造函数私有化
    Route();
    Route(const Route&);
    const Route& operator=(const Route&);

    //单例
    static Route* _instance;
    //单例锁
    static pthread_once_t _once;

    /* ---- 属性 ---- */
    //数据库
    MYSQL _db_conn;  //mysql链接  
    char _sql[1000]; //sql语句

    //modid/cmdid---ip/port 对应的route关系map
    route_map *_data_pointer; //指向RouterDataMap_A 当前的关系map
    route_map *_temp_pointer; //指向RouterDataMap_B 临时的关系map
    pthread_rwlock_t _map_lock;
};
