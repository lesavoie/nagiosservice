#!/bin/bash

LIBS="/usr/lib/cassandra-java-driver-2.0.0/*
/usr/lib/cassandra-java-driver-2.0.0/lib/*"

CP="."

for lib in $LIBS; do
	CP="$lib:$CP"
done

export CLASSPATH=$CLASSPATH:$CP
make cc
java CassandraClient
