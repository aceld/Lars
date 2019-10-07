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
