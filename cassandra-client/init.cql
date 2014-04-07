CREATE KEYSPACE nagioscontrol
    WITH REPLICATION = {'class' : 'SimpleStrategy', 'replication_factor' : 1};
    
USE nagioscontrol;

CREATE TABLE users(
    openid          varchar,
    openid_token    varchar,
    PRIMARY KEY (openid)
);

CREATE TABLE monitor_servers(
    openid  varchar,
    ip_addr varchar,
    PRIMARY KEY (openid, ip_addr)
);    
