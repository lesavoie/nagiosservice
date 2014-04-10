CREATE KEYSPACE nagioscontrol
    WITH REPLICATION = {'class' : 'SimpleStrategy', 'replication_factor' : 1};
    
USE nagioscontrol;

CREATE TABLE control_servers(
    openid  varchar,
    ip_addr varchar,
    PRIMARY KEY (openid)
);    
