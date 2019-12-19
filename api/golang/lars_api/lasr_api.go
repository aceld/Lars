package lars_api

import (
	"fmt"
	"net"
	"strconv"
)

type ip_port struct {
	ip string
	port int
}

type route_set []ip_port

type lars_client interface {
	//lars 系统初始化注册modid/cmdid使用(首次拉取)(初始化使用，只调用一次即可)
	Reg_init(modid int,cmdid int)int

	//lars 系统获取host信息 得到可用host的ip和port
	Get_host(modid int ,  cmdid int, ip string, port int)int

	//lars 系统上报host调用信息
	Report ( modid int,  cmdid int,ip string,  port int,  retcode int)

	//lars 系统获取某modid/cmdid全部的hosts(route)信息
	Get_route( modid int,  cmdid int, route route_set ) int
}


type Lars_client struct {
	Sockfd 	[3]*net.UDPAddr //3个udp socket fd 对应agent 3个udp server
	Seqid   uint32//消息的序列号
}

func Newlars_client ()  (Lc *Lars_client){
	//1 初始化服务器地址 创建监听的地址，并且指定udp协议
	// 默认的ip地址是本地，因为是API和agent应该部署于同一台主机上

	post:=8888
	for i:=0;i<3;i++ {
		Lc.Sockfd[i], err = net.ResolveUDPAddr("udp", "127.0.0.1:"+strconv.Itoa(post+i))
		if err != nil {
			fmt.Println("ResolveUDPAddr err:", err)
			return nil
		}
	}
	return
}

//关闭
func (this *Lars_client) Close() {
	for _, value := range this.Sockfd {
		value
	}
}

//lars 系统初始化注册modid/cmdid使用(首次拉取)(初始化使用，只调用一次即可)
func (this lars_client) Reg_init(modid int,cmdid int)int {
	var route	route_set

	retry_cnt :=0

	for len(route)!=0 && retry_cnt<3{

	}

}


//获取地址
func (this lars_client) Get_host(modid int ,  cmdid int, ip string, port int)int{

}


func (this lars_client)Report ( modid int,  cmdid int,ip string,  port int,  retcode int) {

}
//获取路由
func (this lars_client) Get_route( modid int,  cmdid int, route route_set ) int {

}
