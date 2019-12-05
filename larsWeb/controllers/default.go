package controllers

import (
	"github.com/astaxie/beego"
	"larsWeb/models"
	"time"
	"fmt"
	"larsWeb/utils"
)

type MainController struct {
	beego.Controller
}

//展示登录页
func (this *MainController) ShowLogin() {

	this.TplName = "login.html"
}

//处理数据库登录业务
func (this*MainController)HandleLogin(){
	dbAddr := this.GetString("dbAddr")
	dbPort := this.GetString("dbPort")
	dbUser := this.GetString("dbUser")
	dbPwd := this.GetString("dbPwd")
	dbName := this.GetString("dbName")

	if dbAddr == "" || dbPort == "" || dbUser == "" || dbPwd == "" || dbName == "" {
		this.Data["errmsg"] = "输入数据不完整"
		this.TplName = "login.html"
		return
	}

	//初始化数据库
	if err := models.InitModel(dbAddr,dbPort,dbUser,dbPwd,dbName);err != nil {
		this.Data["errmsg"] = "数据库链接失败,请检查数据库参数"
		this.TplName = "login.html"
		return
	}

	//如果链接成功,把数据存储到session中
	this.SetSession("dbAddr",dbAddr)
	this.SetSession("dbPort",dbPort)
	this.SetSession("dbUser",dbUser)
	this.SetSession("dbPwd",dbPwd)
	this.SetSession("dbName",dbName)

	if err := models.InitModel(dbAddr,dbPort,dbUser,dbPwd,dbName);err != nil {
		this.Redirect("/",302)
		return
	}

	//跳转
	this.Redirect("/db/index",302)
}

//退出登录
func (this*MainController)Logout(){
	this.DelSession("dbAddr")
	this.DelSession("dbPort")
	this.DelSession("dbUser")
	this.DelSession("dbPwd")
	this.DelSession("dbName")

	this.Redirect("/",302)
}

//展示首页
func (this*MainController)ShowIndex(){
	//获取所有主机调动状态
	status,err := models.GetAllStatus()
	if err != nil {
		this.Data["errmsg"] = "当前无主机调动状态"
		this.Layout = "layout.html"
		this.TplName = "index.html"
		return
	}

	for k,v := range status {
		status[k].Time = time.Unix(int64(v.Ts),0)
		status[k].CallerString = utils.InetNtoA(int64(v.Caller))
		status[k].IpString = utils.InetNtoA(int64(v.Ip))

	}

	this.Data["errmsg"] = ""
	this.Data["status"] = status
	this.Layout = "layout.html"
	this.TplName = "index.html"
}

//展示远程主机配置
func (this*MainController)ShowRouteData()  {
	routes,err := models.GetAllRoute()
	if err != nil{
		this.Data["errmsg"] = "当前无主机调动状态"
		this.Layout = "layout.html"
		this.TplName = "routeData.html"
		return
	}

	for k,v := range routes{
		routes[k].IpString = utils.InetNtoA(int64(v.Serverip))
	}

	this.Data["errmsg"] = ""
	this.Data["routes"] = routes
	this.Layout = "layout.html"
	this.TplName = "routeData.html"
}

//删除主机配置
func (this*MainController)HandlerDelete(){
	id,err := this.GetInt("id")
	if err != nil {
		return
	}

	err = models.DeleteRoute(id)
	if err != nil {
		fmt.Println("删除失败")
	}
	this.Redirect("/db/routeData",302)
}

//展示添加主机配置页面
func (this*MainController)ShowAddRoute(){
	this.Layout = "layout.html"
	this.TplName = "add.html"
}

//添加主机配置
func (this*MainController)HandleAddRoute(){
	modid,err1 := this.GetInt("modid")
	cmdid,err2  := this.GetInt("cmdid")
	serverip := this.GetString("serverip")
	serverport,err3 := this.GetInt("serverport")

	if err1 != nil || err2 != nil || serverip == "" || err3 != nil {
		this.Data["errmsg"] = "插入数据错误,请重新填写"
		this.Layout = "layout.html"
		this.TplName = "add.html"
		return
	}

	//把ip转换为整型  需完善
	ipInt := utils.InetAtoN(serverip)


	if err := models.AddRoute(modid,cmdid,int(ipInt),serverport);err != nil {
		this.Data["errmsg"] = "插入数据失败,请重新填写"
		fmt.Println("111111",err)
		this.Layout = "layout.html"
		this.TplName = "add.html"
		return
	}


    //更新修改日志和版本信息
    err := models.InsertRouteChange(modid, cmdid)
	fmt.Println("insert Route Change over",err)
    err = models.UpdateVersion()
	fmt.Println("Update Route Version over",err)

	//插入成功跳转
	this.Redirect("/db/routeData",302)
}

//展示编辑主机配置页面
func (this*MainController)ShowUpdateRoute(){
	id,err := this.GetInt("id")
	if err != nil {
		fmt.Println("获取id数据错误")
		this.Redirect("/db/routeData",302)
		return
	}

	routeData,err := models.GetRouteById(id)
	if err != nil {
		fmt.Println("获取主机数据错误")
		this.Redirect("/db/routeData",302)
		return
	}

	routeData.IpString = utils.InetNtoA(int64(routeData.Serverip))


	this.Data["route"] = routeData
	this.Layout = "layout.html"
	this.TplName = "update.html"
}

//处理更新主机
func (this*MainController)HandleUpdate(){

	id,err :=this.GetInt("id")
	modid,err1 := this.GetInt("modid")
	cmdid,err2  := this.GetInt("cmdid")
	serverip := this.GetString("serverip")
	serverport,err3 := this.GetInt("serverport")

	if err != nil ||err1 != nil || err2 != nil || serverip == "" || err3 != nil {
		this.Data["errmsg"] = "插入数据错误,请重新填写"
		this.Redirect("/db/routeData",302)
		return
	}

	//把ip转成整型  需完善
	ipInt := utils.InetAtoN(serverip)


	err = models.UpdateRoute(id,modid,cmdid,int(ipInt),serverport)
	fmt.Println("111",err)
    err = models.InsertRouteChange(modid, cmdid)
	fmt.Println("insert Route Change over",err)
    err = models.UpdateVersion()
	fmt.Println("Update Route Version over",err)

	this.Redirect("/db/routeData",302)
}

//展示远程主机修改
func (this*MainController)ShowRouteChange(){
	//获取数据
	changes,err := models.GetAllChange()
	if err != nil{
		this.Data["errmsg"] = "当前无主机更改数据"
		this.Layout = "layout.html"
		this.TplName = "showChange.html"
		return
	}

	this.Data["errmsg"] = ""
	this.Data["changes"] = changes
	this.Layout = "layout.html"
	this.TplName = "showChange.html"
}

//展示当前版本信息
func (this*MainController)ShowVersion(){
	//获取数据
	versions,err := models.GetAllVersion()
	if err != nil{
		this.Data["errmsg"] = "获取主机版本信息错误"
		this.Layout = "layout.html"
		this.TplName = "showVersion.html"
		return
	}

	this.Data["errmsg"] = ""
	this.Data["versions"] = versions
	this.Layout = "layout.html"
	this.TplName = "showVersion.html"
}

//展示编辑cmdid页面
func (this*MainController)ShowCmdid(){
	//获取所有配置信息
	cmdids ,err:= models.GetCmdid()
	if err != nil{
		this.Data["errmsg"] = "没有配置信息"
	}else{
		this.Data["errmsg"] = ""
		this.Data["cmdids"] = cmdids
	}

	this.Layout = "layout.html"
	this.TplName = "cmdid.html"
}

//展示编辑cmdid页面
func (this*MainController)ShowModid(){

	modids,err := models.GetModid()
	if err != nil{
		this.Data["errmsg"] = "没有配置信息"
	}else{
		this.Data["errmsg"] = ""
		this.Data["modids"] = modids
	}


	this.Layout = "layout.html"
	this.TplName = "modid.html"
}

//展示添加modid页面
func (this*MainController)ShowAddMod(){
	this.Layout = "layout.html"
	this.TplName = "addMod.html"
}

//展示添加子模块id页面
func (this*MainController)ShowAddCmd(){
	this.Layout = "layout.html"
	this.TplName = "addCmd.html"
}

//处理添加modid
func (this*MainController)HandleAddMod(){
	modid,err := this.GetInt("modid")
	content := this.GetString("content")

	if err != nil || content == "" {
		this.Data["errmsg"] = "输入数据不完整"
		this.ShowAddMod()
		return
	}else {
		err := models.AddModid(modid,content)
		if err != nil {
			this.Data["errmsg"] = "输入数据错误"
			this.ShowAddMod()
			return
		}
	}

	this.Redirect("/db/modid",302)

}

//处理添加cmdid
func (this*MainController)HandleAddCmd(){
	cmdid,err := this.GetInt("cmdid")
	content := this.GetString("content")

	if err != nil || content == "" {
		this.Data["errmsg"] = "输入数据不完整"
		this.ShowAddCmd()
		return
	}else {
		err := models.AddCmdid(cmdid,content)
		if err != nil {
			this.Data["errmsg"] = "插入数据错误"
			this.ShowAddCmd()
			return
		}
	}

	this.Redirect("/db/cmdid",302)
}

//删除modid
func (this*MainController)DeleteMod(){
	id,_ := this.GetInt("id")
	models.DeleteMod(id)

	this.Redirect("/db/modid",302)
}

//删除cmdid
func (this*MainController)DeleteCmd(){
	id,_ := this.GetInt("id")
	models.DeleteCmd(id)

	this.Redirect("/db/cmdid",302)
}

//展示添加modid页面
func (this*MainController)ShowEditMod(){
	id,_ :=this.GetInt("id")
	mod := models.GetMod(id)

	this.Data["mod"] = mod
	this.Layout = "layout.html"
	this.TplName = "editMod.html"
}

//展示添加子模块id页面
func (this*MainController)ShowEditCmd(){
	id,_ :=this.GetInt("id")
	cmd := models.GetCmd(id)

	this.Data["cmd"] = cmd
	this.Layout = "layout.html"
	this.TplName = "editCmd.html"
}


//编辑mod
func (this*MainController)EditMod(){
	modid,err := this.GetInt("modid")
	content := this.GetString("content")

	//fmt.Println(err,content)
	if err != nil || content == "" {
		this.Data["errmsg"] = "输入数据不完整"
		this.ShowEditMod()
		return
	}else {
		err := models.EditMod(modid,content)
		if err != nil {
			this.Data["errmsg"] = "输入数据错误"
			this.ShowEditMod()
			return
		}
	}

	this.Redirect("/db/modid",302)
}

//编辑cmd
func (this*MainController)EditCmd(){
	cmdid,err := this.GetInt("cmdid")
	content := this.GetString("content")

	if err != nil || content == "" {
		this.Data["errmsg"] = "输入数据不完整"
		this.ShowEditCmd()
		return
	}else {
		err := models.Editcmd(cmdid,content)
		if err != nil {
			this.Data["errmsg"] = "插入数据错误"
			this.ShowEditCmd()
			return
		}
	}

	this.Redirect("/db/cmdid",302)
}




