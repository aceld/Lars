#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "lars_reactor.h"
#include "dns_route.h"
#include "string.h"

using namespace std;

//单例对象
Route * Route::_instance = NULL;

//用于保证创建单例的init方法只执行一次的锁
pthread_once_t Route::_once = PTHREAD_ONCE_INIT;



Route::Route()
{
    //1 初始化锁
    pthread_rwlock_init(&_map_lock, NULL);

    //2 初始化map
    _data_pointer = new route_map();//RouterDataMap_A
    _temp_pointer = new route_map();//RouterDataMap_B

    //3 链接数据库
    this->connect_db();

    //4 查询数据库，创建_data_pointer 与 _temp_pointer 两个map
    this->build_maps();
}

void Route::connect_db() 
{
    // --- mysql数据库配置---
    string db_host = config_file::instance()->GetString("mysql", "db_host", "127.0.0.1");
    short db_port = config_file::instance()->GetNumber("mysql", "db_port", 3306);
    string db_user = config_file::instance()->GetString("mysql", "db_user", "root");
    string db_passwd = config_file::instance()->GetString("mysql", "db_passwd", "aceld");
    string db_name = config_file::instance()->GetString("mysql", "db_name", "lars_dns");

    mysql_init(&_db_conn);

    //超时断开
    mysql_options(&_db_conn, MYSQL_OPT_CONNECT_TIMEOUT, "30");
    //设置mysql链接断开后自动重连
    my_bool reconnect = 1; 
    mysql_options(&_db_conn, MYSQL_OPT_RECONNECT, &reconnect);

    if (!mysql_real_connect(&_db_conn, db_host.c_str(), db_user.c_str(), db_passwd.c_str(), db_name.c_str(), db_port, NULL, 0)) {
        fprintf(stderr, "Failed to connect mysql\n");
        exit(1);
    }
}

void Route::build_maps()
{
    int ret = 0;

    snprintf(_sql, 1000, "SELECT * FROM RouteData;");
    ret = mysql_real_query(&_db_conn, _sql, strlen(_sql));
    if ( ret != 0) {
        fprintf(stderr, "failed to find any data, error %s\n", mysql_error(&_db_conn));
        exit(1);
    }

    //得到结果集
    MYSQL_RES *result = mysql_store_result(&_db_conn);
    
    //得到行数
    long line_num = mysql_num_rows(result);

    MYSQL_ROW row;
    for (long i = 0; i < line_num; i++) {
        row = mysql_fetch_row(result);
        int modID = atoi(row[1]);
        int cmdID = atoi(row[2]);
        unsigned ip = atoi(row[3]);
        int port = atoi(row[4]);

        //组装map的key，有modID/cmdID组合
        uint64_t key = ((uint64_t)modID << 32) + cmdID;
        uint64_t value = ((uint64_t)ip << 32) + port;

        printf("modID = %d, cmdID = %d, ip = %u, port = %d\n", modID, cmdID, ip, port);

        //插入到RouterDataMap_A中
        (*_data_pointer)[key].insert(value);
    }

    mysql_free_result(result);
}
