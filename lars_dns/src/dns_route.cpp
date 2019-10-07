#include "dns_route.h"

//单例对象
Route * Route::_instance = NULL;

//用于保证创建单例的init方法只执行一次的锁
pthread_once_t Route::_once = PTHREAD_ONCE_INIT;

Route::Route() 
{

}

