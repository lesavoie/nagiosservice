JDKPATH = /usr/lib/jvm/jdk1.8.0/include
CFLAGS = \
        -I$(JDKPATH)
        
CC = gcc
CNAME = ccproxy


SRC = \
        ccproxy.c
        
space :=
space +=
join-with = $(subst $(space),$1,$(strip $2))
CASSDRIVER = \
		/usr/lib/cassandra-java-driver-2.0.0/* \
		/usr/lib/cassandra-java-driver-2.0.0/lib/*		
		
JFLAGS = \
		-cp $(call join-with,:,$(CASSDRIVER)):.
JC = javac

CLASSES = \
        CassandraClient.java
        
default: ccproxy

ccproxy: $(SRC)
	$(CC) $(CFLAGS) -o $(CNAME) $(SRC)
	
cc: $(CLASSES)
	$(JC) $(CLASSES)

clean:
	rm -f *.o core     
