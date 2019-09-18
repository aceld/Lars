# Lars
(**L**oad balance **A**nd **R**emote service schedule **S**ystem)

---
**【Lars Golang版本移植志愿者招募中】**

QQ:158069367

QQ技术群:[**Go与区块链**]: 135991862

---


# 系统开发环境:
`Linux` : Ubuntu18.04

`protobuf` : libprotoc 3.6.1版本及以上

`mysql`: mysql  Ver 14.14 Distrib 5.7.27 版本及以上

`g++`:  (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0 版本及以上

# 开发技术文档及教程同步更新中...

[《Lars-基于C++负载均衡远程服务器调度系统教程》](https://www.jianshu.com/p/ca3849716cd7)

# 一、概述：

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

![Lars-场景1.png](https://upload-images.jianshu.io/upload_images/11093205-6d98e567922e4522.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### (2)、跨可用区同城容灾

​		弹性负载均衡可将流量跨可用区进行分发，建立实时的同城容灾机制，满足银行贸易等企业对系统的高可用性要求。

#### 优势

- 灵活扩展

  可根据实际的用户访问量，自动扩展负载分发能力

- 同城容灾

  支持跨可用区的双活容灾，实现无缝实时切换


![Lars-场景2.png](https://upload-images.jianshu.io/upload_images/11093205-ba73631bdfd01340.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



### (3)、电商抢购

​		电商业务呈现出较强的潮汐效应。Lars通过和弹性伸缩等服务的无缝集成，自动创建后端云服务器，将流量自动分发到新的云服务器，缓解了促销高峰时期的系统压力。

#### 优势

- 弹性伸缩

  根据业务流量实时创建或移除云服务器

- 高可用

  通过健康检查快速屏蔽异常云服务器，确保业务高可用

- 高性能

  集群支持高并发连接，满足海量访问量诉求

![Lars-场景3.png](https://upload-images.jianshu.io/upload_images/11093205-3a44e779b2663c80.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 3) Lars系统总体架构

​		对于一个部门的后台，为增强灵活性，一个服务可以被抽象为命令字：`modid+cmdid`的组合，称为**一个模块**，而这个服务往往有多个服务节点，其所有服务节点的地址集合被称为这个模块下的**路由**，节点地址简称为节点

- `modid`：标识业务的大类，如：“直播列表相关”

- `cmdid`：标识具体服务内容，如：“批量获取直播列表”

  ​	业务代码利用modid,cmdid，就可以调用对应的远程服务一个Lars系统包含一个DNSService，一个Report Service，以及部署于每个服务器的LoadBalance Agent，业务代码通过API与ELB系统进行交互

**API** ：根据自身需要的`modid,cmdid`，向ELB系统获取节点、汇报节点调用结果；提供`C++`、`Java`、`Python`接口

**LoadBalance Agent**：运行于每个服务器上，负责为此服务器上的业务提供节点获取、节点状态汇报、路由管理、负载调度等核心功能

**DNSService** ： 运行于一台服务器上（也可以用LVS部署多实例防单点），负责`modid,cmdid`到节点路由的转换




**Report Service** ： 运行于DNSService同机服务器上，负责收集各`modid,cmdid`下各节点调用状况，可用于观察、报警

`modid,cmdid`数据由`Mysql`管理，具体SQL脚本在`common/sql`路径下
至于`modid,cmdid`的注册、删除可以利用Web端操作MySQL。

![1-Lars-总体架构设计.png](https://upload-images.jianshu.io/upload_images/11093205-3be2300436781e0f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


如图，每个服务器（虚线）部署了一台LoadBalance Agent，以及多个业务服务

1. 开发者在Web端注册、删除、修改`modid,cmdid`的路由信息，信息被写入到MySQL数据库；
2. 服务器上每个业务biz都把持着自己需要通信的远程服务标识`modid+cmdid`，每个biz都向本机LoadBalance Agent获取远程节点，进而可以和远程目标服务通信，此外业务模块会汇报本次的节点调用结果给LoadBalance Agent；
3. LoadBalance Agent负责路由管理、负载均衡等核心任务，并周期性向DNSService获取最新的路由信息，周期性把各`modid,cmdid`的各节点一段时间内的调用结果传给Report Service
4. DNSService监控MySQL，周期性将最新路由信息加载出来；
5. Report Service将各`modid,cmdid`的各节点一段时间内的调用结果写回到MySQL，方便Web端查看、报警。
