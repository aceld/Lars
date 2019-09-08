#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "io_buf.h"

//构造，创建一个io_buf对象
io_buf::io_buf(int size):
capacity(size), 
length(0),
head(0),
next(NULL) 
{
   data = new char[size];
   assert(data);
}

//清空数据
void io_buf::clear() {
    length = head = 0;
}

//将已经处理过的数据，清空,将未处理的数据提前至数据首地址
void io_buf::adjust() {
    if (head != 0) {
        if (length != 0) {
            memmove(data, data+head, length);
        }
        head = 0;
    }
}

//将其他io_buf对象数据考本到自己中
void io_buf::copy(const io_buf *other) {
    memcpy(data, other->data + other->head, other->length);
    head = 0;
    length = other->length;
}

//处理长度为len的数据，移动head和修正length
void io_buf::pop(int len) {
    length -= len;
    head += len;
}


