USE lars_dns;

SET @time = UNIX_TIMESTAMP(NOW());

INSERT INTO RouteData(modid, cmdid, serverip, serverport) VALUES(1, 1, 3232235953, 9999);
UPDATE RouteVersion SET version = @time WHERE id = 1;

INSERT INTO RouteChange(modid, cmdid, version) VALUES(1, 1, @time);
