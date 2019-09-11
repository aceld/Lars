#pragma once

//解决tcp粘包问题的消息头
struct msg_head
{
    int msgid;
    int msglen;
};

//消息头的二进制长度，固定数值
#define MESSAGE_HEAD_LEN 8

//消息头+消息体的最大长度限制
#define MESSAGE_LENGTH_LIMIT (65535 - MESSAGE_HEAD_LEN)
