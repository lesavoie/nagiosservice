Requirements:
(1) Cassandra

Setup:
(1) Start Cassandra, in Cassandra directory:
	\<cassandra-dir\>/bin/cassandra -f
	
(2) Create keyspace (schema) and tables:
	\<cassandra-dir\>/bin/cqlsh -f init.cql
	
(3) Run the Cassandra client in the background
	nohup java -jar nagios-cassandra-client.jar [options] \<ip\> \<port\> > cclient.out 2> cclient.err < /dev/null &

Using the jar:
Usage: java -jar nagios-cassandra-client.jar [-l username] [-i password] [-h cassandra_ip] [-p cassandra_port] \<listen ip address\> \<port number\>

Protocol to communicate with the Cassandra client:
Tables:
control_servers

Send							Receive
get:\<table\>:<\key\>				get:\<value\>			(value = empty string if key doesn't exist in table)
put:\<table\>:\<key\>:\<value\>		put:[true|false]
update:\<table\>:\<key\>:\<value\>	update:[true|false]
delete:\<table\>:\<key\>			delete:[true|false]
