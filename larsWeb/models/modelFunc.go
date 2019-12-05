package models


import (
	_ "github.com/go-sql-driver/mysql"
	"github.com/jinzhu/gorm"
    "time"
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

	return GlobalDB.Model(new(RouteData)).Where("id = ?",id).
		Updates(map[string]interface{}{"modid":modid,"cmdid":cmdid,"serverip":serverip,"serverport":serverPort}).Error
}

//插入RouteChange信息
func InsertRouteChange(modid, cmdid int) error {
    var routeChange RouteChange
    routeChange.Modid = modid
    routeChange.Cmdid = cmdid
    routeChange.Version = int(time.Now().Unix())

    return GlobalDB.Create(&routeChange).Error
}

//更新版本信息
func UpdateVersion() error {
    return GlobalDB.Model(new(RouteVersion)).Where("id = ?", 1).Updates(map[string]interface{}{"version": int(time.Now().Unix())}).Error
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

//获取所有cmdid配置信息
func GetCmdid()([]CmdIDConfig,error){
	var cmdids []CmdIDConfig
	err := GlobalDB.Find(&cmdids).Error
	return cmdids,err
}

//获取所有modid配置信息
func GetModid()([]ModIDConfig,error){
	var modids []ModIDConfig
	err := GlobalDB.Find(&modids).Error
	return modids,err
}

//添加modid操作
func AddModid(modid int,content string)error{
	var mod ModIDConfig
	mod.Modid = modid
	mod.Content = content
	return GlobalDB.Create(&mod).Error
}

//添加cmdid操作
func AddCmdid(cmdid int,content string)error{
	cmd := CmdIDConfig{Cmdid:cmdid,Content:content}
	return GlobalDB.Create(&cmd).Error
}

//删除mod
func DeleteMod(modid int)error{
	return GlobalDB.Where("id = ?",modid).Delete(new(ModIDConfig)).Error
}

//删除cmd
func DeleteCmd(cmdid int)error{
	return GlobalDB.Where("id = ?",cmdid).Delete(new(CmdIDConfig)).Error
}

//根据id获取mod信息
func GetMod(id int)ModIDConfig{
	var mod ModIDConfig
	GlobalDB.Where("id = ?",id).Find(&mod)
	return mod
}

//根据id获取cmd信息
func GetCmd(id int)CmdIDConfig{
	var cmd CmdIDConfig
	GlobalDB.Where("id = ?",id).Find(&cmd)
	return cmd
}

//编辑mod
func EditMod(modid int,content string)error{
	return GlobalDB.Model(new(ModIDConfig)).Where("modid = ?",modid).
		Update("content",content).Error
}

//编辑cmd
func Editcmd(cmd int,content string)error{
	return GlobalDB.Model(new(CmdIDConfig)).Where("cmdid = ?",cmd).
		Update("content",content).Error
}
