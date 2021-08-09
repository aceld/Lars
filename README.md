# <img width="80px" src="https://s2.ax1x.com/2019/10/09/u4RfDf.png" /> 

(**L**oad balance **A**nd **R**emote service schedule **S**ystem)

[![License](https://img.shields.io/badge/License-MIT%20-blue.svg)](LICENSE) [![Gitter](https://img.shields.io/badge/在线交流-Gitter-green.svg)](https://gitter.im/lars_cpp/community) [![Lars详细教程](https://img.shields.io/badge/Lars详细教程-简书-red.svg)](https://www.jianshu.com/p/ca3849716cd7)
### 开发者
* 刘丹冰([@aceld](https://github.com/aceld))
* 王勇进([@MrWnag](https://github.com/MrWnag))
---
### Github
Git: https://github.com/aceld/Lars

### 码云(Gitee)
Git: https://gitee.com/Aceld/Lars

---


# 一、系统开发环境:
`Linux` : Ubuntu18.04

`protobuf` : libprotoc 3.6.1版本及以上

`mysql`: mysql  Ver 14.14 Distrib 5.7.27 版本及以上

`g++`:  (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0 版本及以上

# 二、开发技术文档及教程
[![Lars详细教程](https://img.shields.io/badge/Lars详细教程-简书-red.svg)](https://www.jianshu.com/p/ca3849716cd7)

* Lars教程 各版本迭代代码:

https://github.com/aceld/Lars_release


### PC端文档

* 看云
## [《Lars-基于C++负载均衡远程服务器调度系统教程》](https://www.kancloud.cn/aceld/lars) 

* 简书
## [《Lars-基于C++负载均衡远程服务器调度系统教程》](https://www.jianshu.com/p/ca3849716cd7) 

![封面](https://upload-images.jianshu.io/upload_images/11093205-5f0b2c8f17dee274.png?imageMogr2/auto-orient/strip|imageView2/2/w/1010/format/webp)

### 移动端文档

![gongzhonghao](https://ww1.yunjiexi.club/2019/12/10/5WjHX.jpg)


# 三、概述：

Lars是一个简单、易用、高性能的服务间远程调用管理、调度、负载均衡系统。

## 1) 优势

1. **性能强悍**

   集群支持千万并发链接，满足用户的海量业务访问需求。

2. **高可用**

   采用集群化部署，支持多可用区的容灾，无缝实时切换。

3. **灵活扩展**

   自动分发，与弹性伸缩无缝集成，灵活扩展用户用于的对外服务能力。

4. **简单易用**

   快速部署、实时生效，支持多种协议，多种调度算法，用户可以高效的管理和调整服务分发策略等。



## 2) 应用场景

### (1)、大型门户网站

​		针对大型门户网站访问量高的特点，通过弹性负载均衡将用户的访问流量均匀的分发到多个后端云服务器上，确保业务快速平稳的运行

#### 优势

- 灵活扩展

  可根据实际的用户访问量，自动扩展负载分发能力

- 高性能

  集群支持高并发连接，满足海量访问量诉求

![Lars-1.png](https://ftp.bmp.ovh/imgs/2019/12/3be0e1bd1e73523a.png)

### (2)、跨可用区同城容灾

​		弹性负载均衡可将流量跨可用区进行分发，建立实时的同城容灾机制，满足银行贸易等企业对系统的高可用性要求。

#### 优势

- 灵活扩展

  可根据实际的用户访问量，自动扩展负载分发能力

- 同城容灾

  支持跨可用区的双活容灾，实现无缝实时切换


![Lars-场景2.png](https://ftp.bmp.ovh/imgs/2019/12/bc530e9d1514ef94.png)



### (3)、电商抢购

​		电商业务呈现出较强的潮汐效应。Lars通过和弹性伸缩等服务的无缝集成，自动创建后端云服务器，将流量自动分发到新的云服务器，缓解了促销高峰时期的系统压力。

#### 优势

- 弹性伸缩

  根据业务流量实时创建或移除云服务器

- 高可用

  通过健康检查快速屏蔽异常云服务器，确保业务高可用

- 高性能

  集群支持高并发连接，满足海量访问量诉求

![Lars-场景3.png](https://ftp.bmp.ovh/imgs/2019/12/f370141ee37e00bb.png)

## 3) Lars系统总体架构

​		对于一个部门的后台，为增强灵活性，一个服务可以被抽象为命令字：`modid+cmdid`的组合，称为**一个模块**，而这个服务往往有多个服务节点，其所有服务节点的地址集合被称为这个模块下的**路由**，节点地址简称为节点

- `modid`：标识业务的大类，如：“直播列表相关”

- `cmdid`：标识具体服务内容，如：“批量获取直播列表”

  ​	业务代码利用modid,cmdid，就可以调用对应的远程服务一个Lars系统包含一个DNSService，一个Report Service，以及部署于每个服务器的LoadBalance Agent，业务代码通过API与Lars系统进行交互

**API** ：根据自身需要的`modid,cmdid`，向Lars系统获取节点、汇报节点调用结果；提供`C++`、`Golang`(开发中...)、`Python`(开发中...)接口

**LoadBalance Agent**：运行于每个服务器上，负责为此服务器上的业务提供节点获取、节点状态汇报、路由管理、负载调度等核心功能

**DNSService** ： 运行于一台服务器上（也可以用LVS部署多实例防单点），负责`modid,cmdid`到节点路由的转换

**Report Service** ： 运行于DNSService同机服务器上，负责收集各`modid,cmdid`下各节点调用状况，可用于观察、报警

`modid,cmdid`数据由`Mysql`管理，具体SQL脚本在`common/sql`路径下
至于`modid,cmdid`的注册、删除可以利用Web端操作MySQL。

![1-Lars-总体架构设计.png](https://ftp.bmp.ovh/imgs/2019/12/73f562dccbe08f45.png)


如图，每个服务器（虚线）部署了一台LoadBalance Agent，以及多个业务服务

1. 开发者在Web端注册、删除、修改`modid,cmdid`的路由信息，信息被写入到MySQL数据库；
2. 服务器上每个业务biz都把持着自己需要通信的远程服务标识`modid+cmdid`，每个biz都向本机LoadBalance Agent获取远程节点，进而可以和远程目标服务通信，此外业务模块会汇报本次的节点调用结果给LoadBalance Agent；
3. LoadBalance Agent负责路由管理、负载均衡等核心任务，并周期性向DNSService获取最新的路由信息，周期性把各`modid,cmdid`的各节点一段时间内的调用结果传给Report Service
4. DNSService监控MySQL，周期性将最新路由信息加载出来；
5. Report Service将各`modid,cmdid`的各节点一段时间内的调用结果写回到MySQL，方便Web端查看、报警。

# 四、快速开始
## (1) 编译及安装
**代码下载**
```bash
    git clone https://github.com/aceld/Lars.git
```
**编译**
```bash
    cd ./Lars
    make
```

> 注意：Lars依赖protobuf库，所以环境应该有protobuf环境，如果没有，可以参考下面文章进行安装
> https://mp.weixin.qq.com/s/c--hV7AXMKnbuUAm59_DvA


## (2) 数据库配置
**创建表**
```bash
    cd ./Lars/base/sql
```
    进入`mysql`，导入表`lars_dns.sql`文件
```bash
    mysql -u root -p
    #输入密码
```
```bash
mysql> source ./lars_dns.sql
```
## (3) 各子系统配置文档
**lars reporter**
> ./Lars/lars_reporter/conf/lars_reporter.conf 
```ini
[reactor]
maxConn = 1024
threadNum = 5
ip = 127.0.0.1
port = 7779

[mysql]
db_host = 127.0.0.1
db_port = 3306
db_user = root
db_passwd = **Your PassWord**
db_name = lars_dns

[repoter]
db_thread_cnt = 3    
```


**lars dns**
> ./Lars/lars_dns/conf/lars_dns.conf
```ini
[reactor]
maxConn = 1024
threadNum = 5
ip = 127.0.0.1
port = 7778

[mysql]
db_host = 127.0.0.1
db_port = 3306
db_user = root
db_passwd = **Your PassWord**
db_name = lars_dns
```

**lars LoadBalance Agent**
> ./Lars/lars_loadbalance_agent/conf/lars_lb_agent.conf
```ini
[reporter]
ip = 127.0.0.1
port = 7779

[dnsserver]
ip = 127.0.0.1
port = 7778

[loadbalance]

;经过若干次获取请求host节点后，试探选择一次overload过载节点
probe_num=10

;初始化host_info主机信息访问成功的个数，防止刚启动时少量失败就认为过载
init_succ_cnt=180

;当idle节点切换至over_load时的初始化失败次数，主要为了累计一定成功次数才能切换会idle
init_err_cnt=5

;当idle节点失败率高于此值，节点变overload状态
err_rate=0.1

;当overload节点成功率高于此值，节点变成idle状态
succ_rate=0.5

;当idle节点连续失败次数超过此值，节点变成overload状态
contin_err_limit=15

;当overload节点连续成功次数超过此值, 节点变成idle状态
contin_succ_limit=15

;整个窗口的真实失败率阈值
window_err_rate=0.7

;对于某个modid/cmdid下的某个idle状态的host，需要清理一次负载信息的周期
idle_timeout=15

;对于某个modid/cmdid/下的某个overload状态的host，在过载队列等待的最大时间
overload_timeout=15

;对于每个NEW状态的modid/cmdid，多久更新一下本地路由,秒
update_timeout=15
```

## (4)启动
**启动 lars reporter serivce**
```bash
    cd ./Lars
    ./run_lars reporter
```

**启动 lars dns service**
```bash
    cd ./Lars
    ./run_lars dns
```

**启动 lars lbagent service**
```bash
    cd ./Lars
    ./run_lars lbagent
```

**启动 lars web service**
```bash
    cd ./Lars
    ./run_lars web
```

## (5) Web管理端操作
**打开浏览器,输入web 服务的ip地址+端口号**
![lars_web_login.png](https://s2.ax1x.com/2019/11/29/QA8Zh4.png)

**配置主机**
![lars_web_add_host.png](https://s2.ax1x.com/2019/11/29/QAGoLt.png)

**查看所有主机配置**
![lars_web_all_hosts.png](https://s2.ax1x.com/2019/11/29/QAJvtO.png)

## (6) 模拟效果测试
**启动模拟器测试**
```bash
    cd ./Lars
    ./run_lars test simulator 1 1
```

**更多测试工具**
```bash
    cd ./Lars
    ./run_lars help
```

```bash

=======启动子系统==========
Usage ./run_lars [reporter|dns|lbagent|web|test]

=======测试工具============
Usage ./run_lars test gethost ModID CmdID
Usage ./run_lars test getroute ModID CmdID
Usage ./run_lars test report ModID CmdID IP Port 0|1  --- 0:SUCC, 1:OVERLOAD
Usage ./run_lars test simulator ModID CmdID [errRate(0-10)] [queryCnt(0-999999)]
Usage ./run_lars test qps ThreadNum
Usage ./run_lars test example ModID CmdID

```

## (7)Web查看主机调用状态

![lars_web_status.png](https://s2.ax1x.com/2019/11/29/QAteVx.png)

# 五、Lars QPS性能测试
**主机1**
> CPU个数：2个 ， 内存: 2GB ,   系统：Ubuntu18.04虚拟机

| 线程数 | QPS    |
| ------ | ------ |
| 1      | 0.5w/s |
| 2      | 2.2w/s |
| 10     | 5.5w/s |
| 100    | 5.3w/s |


**主机2**
> CPU个数: 24个 , 内存:128GB， 系统: 云主机

| 线程数    | QPS | 
| ------ | ------ |
| 1 | 8.36w/s| 
| 3 | 28.06w/s| 
| 5 | 55.18w/s| 
| 8 | 56.74w/s| 

# Lars技术讨论社区

### **微信公众号**
![gongzhonghao](https://s1.ax1x.com/2020/07/07/UFyUdx.th.jpg)


### **QQ资源分享**

![qqqun](https://s1.ax1x.com/2020/07/07/UF6Y9S.th.png)

### **微信群**
<img src="https://s1.ax1x.com/2020/07/07/UF6rNV.png" width = "200" height = "230" alt="weixin" />
欢迎大家加入，一起学习分享

