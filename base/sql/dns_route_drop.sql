USE lars_dns;

DELETE FROM RouteData;
DELETE FROM ServerCallStatus;
DELETE FROM RouteChange;
UPDATE RouteVersion SET version = UNIX_TIMESTAMP(NOW()) WHERE id = 1;
