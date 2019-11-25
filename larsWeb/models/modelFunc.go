package models

import (
	_ "github.com/go-sql-driver/mysql"
	"github.com/jinzhu/gorm"
)

var GlobalDB *gorm.DB

//初始化表结构
func InitModel(sqlAddr,sqlPort,sqlUser,sqlPwd,sqlDb string)error{
	//构造连接字符串
	connstring := sqlUser + ":"+sqlPwd + "@tcp("+sqlAddr+":"+sqlPort+")/"+sqlDb
	db,err := gorm.Open("mysql",connstring)
	if err != nil {
		return err
	}
	GlobalDB = db
	//建表
	db.AutoMigrate(new(RouteData),new(RouteVersion),new(RouteChange),new(ServerCallStatus))

	return err
}

//获取所有调用状态
func GetAllStatus()([]ServerCallStatus,error){
	var status []ServerCallStatus
	if err := GlobalDB.Find(&status).Error;err != nil {
		///fmt.Println("获取数据错误")
		return nil,err
	}
	return status,nil
}

//获取远程主机配置
func GetAllRoute()([]RouteData,error){
	var routeDatas []RouteData
	if err := GlobalDB.Find(&routeDatas).Error;err != nil {
		return nil,err
	}
	return routeDatas,nil
}

//删除远程主机配置
func DeleteRoute(id int)error{
	return GlobalDB.Where("id = ?",id).Delete(new(RouteData)).Error
}

//添加主机信息
func AddRoute(modid,cmdid,serverip,serverPort int)error{
	var routeData RouteData
	routeData.Modid = modid
	routeData.Cmdid = cmdid
	routeData.Serverip = serverip
	routeData.Serverport = serverPort

	return GlobalDB.Create(&routeData).Error
}

//根据ip获取主机信息
func GetRouteById(id int)(RouteData,error){
	var routeData RouteData
	return routeData,GlobalDB.Where("id = ?",id).Find(&routeData).Error
}

//更新主机信息
func UpdateRoute(id,modid,cmdid,serverip,serverPort int)error{

	return GlobalDB.Where("id = ?",id).
		Updates(map[string]interface{}{"modid":modid,"cmdid":cmdid,"serverip":serverip,"serverport":serverPort}).Error
}

//获取远程主机修改
func GetAllChange()([]RouteChange,error){
	var routeChanges []RouteChange
	if err := GlobalDB.Find(&routeChanges).Error;err != nil {
		return nil,err
	}
	return routeChanges,nil
}

//获取所有版本信息
func GetAllVersion()([]RouteVersion,error){
	var routeVersions []RouteVersion
	if err := GlobalDB.Find(&routeVersions).Error;err != nil {
		return nil,err
	}
	return routeVersions,nil
}