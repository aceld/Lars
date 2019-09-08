#include "buf_pool.h"
#include <assert.h>


//单例对象
buf_pool * buf_pool::_instance = NULL;

//用于保证创建单例的init方法只执行一次的锁
pthread_once_t buf_pool::_once = PTHREAD_ONCE_INIT;


//用户保护内存池链表修改的互斥锁
pthread_mutex_t buf_pool::_mutex = PTHREAD_MUTEX_INITIALIZER;


//构造函数 主要是预先开辟一定量的空间
//这里buf_pool是一个hash，每个key都是不同空间容量
//对应的value是一个io_buf集合的链表
//buf_pool -->  [m4K] -- io_buf-io_buf-io_buf-io_buf...
//              [m16K] -- io_buf-io_buf-io_buf-io_buf...
//              [m64K] -- io_buf-io_buf-io_buf-io_buf...
//              [m256K] -- io_buf-io_buf-io_buf-io_buf...
//              [m1M] -- io_buf-io_buf-io_buf-io_buf...
//              [m4M] -- io_buf-io_buf-io_buf-io_buf...
//              [m8M] -- io_buf-io_buf-io_buf-io_buf...
buf_pool::buf_pool():_total_mem(0)
{
    io_buf *prev; 
    
    //----> 开辟4K buf 内存池
    _pool[m4K] = new io_buf(m4K);
    if (_pool[m4K] == NULL) {
        fprintf(stderr, "new io_buf m4K error");
        exit(1);
    }

    prev = _pool[m4K];
    //4K的io_buf 预先开辟5000个，约20MB供开发者使用
    for (int i = 1; i < 5000; i ++) {
        prev->next = new io_buf(m4K);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m4K error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 4 * 5000;



    //----> 开辟16K buf 内存池
    _pool[m16K] = new io_buf(m16K);
    if (_pool[m16K] == NULL) {
        fprintf(stderr, "new io_buf m16K error");
        exit(1);
    }

    prev = _pool[m16K];
    //16K的io_buf 预先开辟1000个，约16MB供开发者使用
    for (int i = 1; i < 1000; i ++) {
        prev->next = new io_buf(m16K);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m16K error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 16 * 1000;



    //----> 开辟64K buf 内存池
    _pool[m64K] = new io_buf(m64K);
    if (_pool[m64K] == NULL) {
        fprintf(stderr, "new io_buf m64K error");
        exit(1);
    }

    prev = _pool[m64K];
    //64K的io_buf 预先开辟500个，约32MB供开发者使用
    for (int i = 1; i < 500; i ++) {
        prev->next = new io_buf(m64K);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m64K error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 64 * 500;


    //----> 开辟256K buf 内存池
    _pool[m256K] = new io_buf(m256K);
    if (_pool[m256K] == NULL) {
        fprintf(stderr, "new io_buf m256K error");
        exit(1);
    }

    prev = _pool[m256K];
    //256K的io_buf 预先开辟200个，约50MB供开发者使用
    for (int i = 1; i < 200; i ++) {
        prev->next = new io_buf(m256K);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m256K error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 256 * 200;


    //----> 开辟1M buf 内存池
    _pool[m1M] = new io_buf(m1M);
    if (_pool[m1M] == NULL) {
        fprintf(stderr, "new io_buf m1M error");
        exit(1);
    }

    prev = _pool[m1M];
    //1M的io_buf 预先开辟50个，约50MB供开发者使用
    for (int i = 1; i < 50; i ++) {
        prev->next = new io_buf(m1M);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m1M error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 1024 * 50;


    //----> 开辟4M buf 内存池
    _pool[m4M] = new io_buf(m4M);
    if (_pool[m4M] == NULL) {
        fprintf(stderr, "new io_buf m4M error");
        exit(1);
    }

    prev = _pool[m4M];
    //4M的io_buf 预先开辟20个，约80MB供开发者使用
    for (int i = 1; i < 20; i ++) {
        prev->next = new io_buf(m4M);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m4M error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 4096 * 20;



    //----> 开辟8M buf 内存池
    _pool[m8M] = new io_buf(m8M);
    if (_pool[m8M] == NULL) {
        fprintf(stderr, "new io_buf m8M error");
        exit(1);
    }

    prev = _pool[m8M];
    //8M的io_buf 预先开辟10个，约80MB供开发者使用
    for (int i = 1; i < 10; i ++) {
        prev->next = new io_buf(m8M);
        if (prev->next == NULL) {
            fprintf(stderr, "new io_buf m8M error");
            exit(1);
        }
        prev = prev->next;
    }
    _total_mem += 8192 * 10;
}


//开辟一个io_buf
//1 如果上层需要N个字节的大小的空间，找到与N最接近的buf hash组，取出，
//2 如果该组已经没有节点使用，可以额外申请
//3 总申请长度不能够超过最大的限制大小 EXTRA_MEM_LIMIT
//4 如果有该节点需要的内存块，直接取出，并且将该内存块从pool摘除
io_buf *buf_pool::alloc_buf(int N) 
{
    //1 找到N最接近哪hash 组
    int index;
    if (N <= m4K) {
        index = m4K;
    }
    else if (N <= m16K) {
        index = m16K;
    }
    else if (N <= m64K) {
        index = m64K;
    }
    else if (N <= m256K) {
        index = m256K;
    }
    else if (N <= m1M) {
        index = m1M;
    }
    else if (N <= m4M) {
        index = m4M;
    }
    else if (N <= m8M) {
        index = m8M;
    }
    else {
        return NULL;
    }


    //2 如果该组已经没有，需要额外申请，那么需要加锁保护
    pthread_mutex_lock(&_mutex);
    if (_pool[index] == NULL) {
        if (_total_mem + index/1024 >= EXTRA_MEM_LIMIT) {
            //当前的开辟的空间已经超过最大限制
            fprintf(stderr, "already use too many memory!\n");
            exit(1);
        }

        io_buf *new_buf = new io_buf(index);
        if (new_buf == NULL) {
            fprintf(stderr, "new io_buf error\n");
            exit(1);
        }
        _total_mem += index/1024;
        pthread_mutex_unlock(&_mutex);
        return new_buf;
    }

    //3 从pool中摘除该内存块
    io_buf *target = _pool[index];
    _pool[index] = target->next;
    pthread_mutex_unlock(&_mutex);
    
    target->next = NULL;
    
    return target;
}


//重置一个io_buf,将一个buf 上层不再使用，或者使用完成之后，需要将该buf放回pool中
void buf_pool::revert(io_buf *buffer)
{
    //每个buf的容量都是固定的 在hash的key中取值
    int index = buffer->capacity;
    //重置io_buf中的内置位置指针
    buffer->length = 0;
    buffer->head = 0;

    pthread_mutex_lock(&_mutex);
    //找到对应的hash组 buf首届点地址
    assert(_pool.find(index) != _pool.end());

    //将buffer插回链表头部
    buffer->next = _pool[index];
    _pool[index] = buffer;

    pthread_mutex_unlock(&_mutex);
}
