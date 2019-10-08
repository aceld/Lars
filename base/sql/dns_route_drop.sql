USE lars_dns;

DELETE FROM RouteData;
UPDATE RouteVersion SET version = UNIX_TIMESTAMP(NOW()) WHERE id = 1;
