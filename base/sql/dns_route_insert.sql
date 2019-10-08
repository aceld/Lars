USE lars_dns;

INSERT INTO RouteData(modid, cmdid, serverip, serverport) VALUES(1, 1, 3232235953, 7777);
INSERT INTO RouteData(modid, cmdid, serverip, serverport) VALUES(1, 2, 3232235954, 7776);
INSERT INTO RouteData(modid, cmdid, serverip, serverport) VALUES(1, 2, 3232235955, 7778);
INSERT INTO RouteData(modid, cmdid, serverip, serverport) VALUES(1, 2, 3232235956, 7779);

UPDATE RouteVersion SET version = UNIX_TIMESTAMP(NOW()) WHERE id = 1;
