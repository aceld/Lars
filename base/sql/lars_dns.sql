DROP DATABASE if exists lars_dns;
CREATE DATABASE lars_dns;
USE lars_dns;

DROP TABLE IF EXISTS `RouteData`;
CREATE TABLE `RouteData` (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `modid` int(10) unsigned NOT NULL,
    `cmdid` int(10) unsigned NOT NULL,
    `serverip` int(10) unsigned NOT NULL,
    `serverport` int(10) unsigned NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=116064 DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `RouteVersion`;
CREATE TABLE RouteVersion (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `version` int(10) unsigned NOT NULL,
    PRIMARY KEY (`id`)
);
INSERT INTO RouteVersion(version) VALUES(0);


DROP TABLE IF EXISTS `RouteChange`;
CREATE TABLE RouteChange (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `modid` int(10) unsigned NOT NULL,
    `cmdid` int(10) unsigned NOT NULL,
    `version` bigint(20) unsigned NOT NULL,
    PRIMARY KEY (`id`)
);


DROP TABLE IF EXISTS `ServerCallStatus`;
CREATE TABLE `ServerCallStatus` (
      `modid` int(11) NOT NULL,
      `cmdid` int(11) NOT NULL,
      `ip` int(11) unsigned NOT NULL,
      `port` int(11) unsigned NOT NULL,
      `caller` int(11) unsigned NOT NULL,
      `succ_cnt` int(11) NOT NULL,
      `err_cnt` int(11) NOT NULL,
      `ts` bigint(20) NOT NULL,
      `overload` char(1) NOT NULL,
      PRIMARY KEY (`modid`,`cmdid`,`ip`,`port`,`caller`),
      KEY `mlb_index` (`modid`,`cmdid`,`ip`,`port`,`caller`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `ModIDConfig`;
CREATE TABLE ModIDConfig (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `modid` int(10) unsigned NOT NULL,
    `content` char(255) NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `CmdIDConfig`;
CREATE TABLE CmdIDConfig (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `cmdid` int(10) unsigned NOT NULL,
    `content` char(255) NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


