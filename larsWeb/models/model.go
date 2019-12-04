package models

import "time"

type RouteData struct {
	Id         int
	Modid      int
	Cmdid      int
	Serverip   int
	Serverport int
	IpString   string `orm:"-"`
}

func (u *RouteData) TableName() string {
	return "RouteData"
}

type RouteVersion struct {
	Id      int
	Version int
}

func (u *RouteVersion) TableName() string {
	return "RouteVersion"
}

type RouteChange struct {
	Id      int
	Modid   int
	Cmdid   int
	Version int
}

func (u *RouteChange) TableName() string {
	return "RouteChange"
}

type ServerCallStatus struct {
	Modid    int `gorm:"primary_key"`
	Cmdid    int `gorm:"primary_key"`
	Ip       int `gorm:"primary_key"`
	Port     int `gorm:"primary_key"`
	Caller   int `gorm:"primary_key"`
	SuccCnt  int `gorm:"primary_key"`
	ErrCnt   int `gorm:"primary_key"`
	Ts       int `orm:"size(20)"`
	Time     time.Time `orm:"-"`
	Overload bool
	CallerString string `orm:"-"`
	IpString string 	`orm:"-"`
}
func (u *ServerCallStatus) TableName() string {
	return "ServerCallStatus"
}


type ModIDConfig struct {
	Id int `orm:"size(10)"`
	Modid int `orm:"size(10)"`
	Content string
}
func (u *ModIDConfig) TableName() string {
	return "ModIDConfig"
}

type CmdIDConfig struct {
	Id int `orm:"size(10)"`
	Cmdid int `orm:"size(10)"`
	Content string
}
func (u *CmdIDConfig) TableName() string {
	return "CmdIDConfig"
}





