/* Author : Nagios Service Team. 
 * Each packet contains a command, the interpreter
 * contains logic to parse these commands and call
 * the appropriate handlers attached to them.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <connection.h>
#include <interpreter.h>
#include <merrors.h>
#include <mbuffer.h>
#include <lib.h>
#include <log.h>
#include <cconnection.h>

#include <cfuhash.h>

/* These fields are required to speak with the cassandra 
 * database deamon. */
extern char *myip;	/* Ip of the instance that has called this. */
extern char *dbip;	/* Ip address of the cassandra daemon is running on. */
extern char *dbport;
extern char *tablename;

extern char *id;
extern cfuhash_table_t *controller_map;

static int configHost(uint8_t *buf, int size);
static int configHello(uint8_t *buf, int size);
static int configIdent(uint8_t *buf, int size, int fd);
static int inline restartNagios();
static int writeFile(char *filename, uint8_t *buf, int size);

/* Function which takes a command packet, parses it to 
 * determine to which command it belongs and then
 * takes the required actions. */
int parsePacket(uint8_t *buf, int fd) {
	struct Packet *header;	
	int ret = ERR;

	header = (struct Packet *)buf;

	switch(header->type){
		case HOST:
			configHost(buf + sizeof(struct Packet), header->len -
													sizeof(struct Packet));
			break;
		case SERVICE:
			break;
		case CONTACT:
			break;
		case IDENT:
			configIdent(buf + sizeof(struct Packet), header->len -
													sizeof(struct Packet), fd);
			break;
		case HELLO:
			configHello(buf + sizeof(struct Packet), header->len -
													sizeof(struct Packet));
			break;
		default:
			#if DEBUG == 1
				fprintf(stderr, "parsePacket :: Invalid header type\n");
			#endif
			return ERR;
	}

	return ret;
}

/* Function to handle the monitor identification. */
static int configIdent(uint8_t *buf, int size, int fd){
	uint8_t *key = NULL;
	int *data = NULL;	
	char *dbstatus = NULL;

	#if DEBUG == 1
		fprintf(stderr, "Monitor identified as : %s\n", buf);
	#endif

	key = (uint8_t*)malloc(size);
	memcpy(key, buf, size);
	data = (int*)malloc(sizeof(int));
	memcpy(data, &fd, sizeof(int));

	/* Add the mapping of the fd and monitor name to 
	 * controller map. */
	cfuhash_put(controller_map, (char*)key, (void*)data);

	#if DEBUG == 1
		fprintf(stderr, "configIdent :: Connecting to %s %s\n",
												dbip, dbport);
	#endif
	
	/* Also send this mapping to the cassandra database. */
	dbstatus = libDbPut(tablename, (char*)key, (char*)myip, dbip, dbport);	

	#if DEBUG == 1
		cfuhash_pretty_print(controller_map, stderr);
	#endif

	#if DEBUG == 1
		fprintf(stderr, "configIdent :: DB returned => %s\n", dbstatus);
	#endif

	free(dbstatus);
	
	return 0;
}

/* Function to handle changes to the host configuration
 * file. */
static int configHost(uint8_t *buf, int size){

	char *filename = "/etc/nagios3/conf.d/remote_host.cfg";

	#if DEBUG == 1
		fprintf(stderr, "configHost :: Request host configuration\n");
	#endif

	if(writeFile(filename, buf, size) < 0){
		return ERR;	
	}

	restartNagios();

	return  0;
}

static int configService(uint8_t *buf, int size){

	char *filename = "/etc/nagios3/conf.d/remote_service.cfg";

	#if DEBUG == 1
		fprintf(stderr, "configHost :: Request service configuration\n");
	#endif

	if(writeFile(filename, buf, size) < 0){
		return ERR;	
	}

	restartNagios();

	return  0;
}

static int configHello(uint8_t *buf, int size){

	uint8_t *data;

	#if DEBUG == 1
		fprintf(stderr, "configHello :: Request hello packet\n");
	#endif

	/* Reply back with the id of this monitor, which is currently 
	 * just the username. */
	data = newPacket(IDENT, (uint8_t*)id, strlen(id));	
	QAddData(data, (*((struct Packet*)data)).len);	

	return 0;
}

/* Function to write to the given file. */
static int writeFile(char *filename, uint8_t *buf, int size){
	int count = -1;
	int fd = -1;

	if(size < 0 || filename == NULL || buf == NULL){
		#if DEBUG == 1 
			fprintf(stderr, "writeFile :: Argument error\n");
		#endif
		
		return ERR;
	}

	if((fd = open(filename, O_WRONLY, 0)) < 0){
		#if DEBUG == 1
			fprintf(stderr, "configHost :: Could not open config file\n");
		#endif
		
		return ERR;
	}
	
	if((count = write(fd, buf, size)) < 0){
		#if DEBUG == 1 
			fprintf(stderr, "writeFile :: write() failure");
		#endif
		
		close(fd);
		return ERR;
	}

	return count;
}

/* Command to restart the nagios service. */
static int inline restartNagios() {
	return system("service nagios3 restart");
}
