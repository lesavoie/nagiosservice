Requirements:
(1) Cassandra
(2) DataStax Cassandra Java Driver (put in /usr/lib)

Setup:
(1) Start Cassandra, in Cassandra directory:
	<cassandra-dir>/bin/cassandra -f
	
(2) Create keyspace (schema) and tables:
	<cassandra-dir>/bin/cqlsh -f init.cql
	
(3) Setup and run the Cassandra client
	./startup.sh
