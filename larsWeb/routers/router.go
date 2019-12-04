package routers

import (
	"larsWeb/controllers"
	"github.com/astaxie/beego"
	"github.com/astaxie/beego/context"
)

func init() {
	beego.InsertFilter("/db/*",beego.BeforeExec,HandlerFunc)

    beego.Router("/", &controllers.MainController{},"get:ShowLogin")
    //登录页
    beego.Router("/login",&controllers.MainController{},"post:HandleLogin")
    //退出登录
	beego.Router("/logout",&controllers.MainController{},"get:Logout")

	//首页展示
    beego.Router("/db/index",&controllers.MainController{},"get:ShowIndex")
    //展示远程主机配置
    beego.Router("/db/routeData",&controllers.MainController{},"get:ShowRouteData")
    //删除远程主机配置信息
    beego.Router("/db/delete",&controllers.MainController{},"get:HandlerDelete")
    //添加主机配置
    beego.Router("/db/addRouteData",&controllers.MainController{},"get:ShowAddRoute;post:HandleAddRoute")
    //编辑主机配置
    beego.Router("/db/updateRoute",&controllers.MainController{},"get:ShowUpdateRoute;post:HandleUpdate")
    //展示主机更改记录
    beego.Router("/db/showRouteChange",&controllers.MainController{},"get:ShowRouteChange")
    //展示版本信息
    beego.Router("/db/showVersion",&controllers.MainController{},"get:ShowVersion")
    //子模块id
	beego.Router("/db/cmdid",&controllers.MainController{},"get:ShowCmdid")
	//模块id
	beego.Router("/db/modid",&controllers.MainController{},"get:ShowModid")
	//添加模块id
	beego.Router("/db/addMod",&controllers.MainController{},"get:ShowAddMod;post:HandleAddMod")
	//添加子模块id
	beego.Router("/db/addCmd",&controllers.MainController{},"get:ShowAddCmd;post:HandleAddCmd")
	//删除模块id
	beego.Router("/db/deleteMod",&controllers.MainController{},"get:DeleteMod")
	//删除模块id
	beego.Router("/db/deleteCmd",&controllers.MainController{},"get:DeleteCmd")
	//编辑mod
	beego.Router("/db/editMod",&controllers.MainController{},"get:ShowEditMod;post:EditMod")
	//编辑cmd
	beego.Router("/db/editCmd",&controllers.MainController{},"get:ShowEditCmd;post:EditCmd")
}

//登录校验
func HandlerFunc(ctx*context.Context){
	dbAddr :=ctx.Input.Session("dbAddr")
	dbPort :=ctx.Input.Session("dbPort")
	dbUser :=ctx.Input.Session("dbUser")
	dbPwd := ctx.Input.Session("dbPwd")
	dbName :=ctx.Input.Session("dbName")


	if dbAddr == nil || dbPort == nil || dbUser == nil || dbPwd == nil || dbName == nil {
		ctx.Redirect(302,"/")
		return
	}


	//初始化数据库

}