#pragma once

//总内存池最大限制 单位是Kb 所以目前限制是 5GB
#define EXTRA_MEM_LIMIT (5U *1024 *1024) 

/*
    定义一个 buffer存放数据的结构
 * */
class io_buf {
public:
    //构造，创建一个io_buf对象
    io_buf(int size);

    //清空数据
    void clear();

    //将已经处理过的数据，清空,将未处理的数据提前至数据首地址
    void adjust();

    //将其他io_buf对象数据考本到自己中
    void copy(const io_buf *other);

    //处理长度为len的数据，移动head和修正length
    void pop(int len);

    //如果存在多个buffer，是采用链表的形式链接起来
    io_buf *next;

    //当前buffer的缓存容量大小
    int capacity;
    //当前buffer有效数据长度
    int length;
    //未处理数据的头部位置索引
    int head;

private:
    //当前io_buf所保存的数据地址
    char *data;
};

