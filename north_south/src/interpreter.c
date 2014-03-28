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

#include <connection.h>
#include <interpreter.h>
#include <merrors.h>
#include <log.h>

static int configHost(uint8_t *buf, int size);
static int configHello(uint8_t *buf, int size);
static int inline restartNagios();
static int writeFile(char *filename, uint8_t *buf, int size);

/* Function which takes a command packet, parses it to 
 * determine to which command it belongs and then
 * takes the required actions. */
int parsePacket(uint8_t *buf) {
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

	#if DEBUG == 1
		fprintf(stderr, "configHello :: Request hello packet\n");
	#endif

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
