#!/bin/bash

USER="pshen"
CASSANDRA="/home/${USER}/apache-cassandra-2.0.4"

# start cassandra
$CASSANDRA/bin/cassandra

# run the daemon in the background
PORT_NO=6666
nohup java -jar nagios-cassandra-client.jar $PORT_NO > cclient.out 2> cclient.err < /dev/null &
