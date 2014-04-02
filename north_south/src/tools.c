/* Author : Nagios Services team */
/* Tool which provides a CLI to send config files to the
 * cotroller. */

#include <stdlib.h>
#include <unistd.h>
#include <merrors.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#include <lib.h>
#include <connection.h>
#include <log.h>

char *id;

static int pushConfigFile(char *filename, char *ip, char *port,
									char *id, int type);

int main(int argc, char *argv[]){
	int opt;
	char *ip, *port;
	char *filename = NULL;
	int type = -1;

	while((opt = getopt(argc, argv, "c:p:hi:f:t:")) != -1){
		switch(opt) {
			case 'f':
				/* File name to read the config file information from. */
				filename = optarg;
				break;
			case 'c':
				/* IP address of the control server to connect to. */
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'i':
				id = optarg;
				break;
			case 't':
				type = atoi(optarg);
				break;
			case 'h':

			case '?':

			default:
				/* Print the usage message. */
				fprintf(stdout, "Argument error.\n");
				return -1;
		}
	}

	pushConfigFile(filename, ip, port, id, type);
	
	return 0;
}

/* Send the configuration file to the controller. */
static int pushConfigFile(char *filename, char *ip, char *port,
									char *id, int type){

	struct sockaddr_in address;
	int sd = -1;
	int fd = -1;
	uint8_t *data = NULL;
	int count = -1;
	int offset = -1;
	uint8_t *packet = NULL;
	char *newid = NULL;

	if(filename == NULL || ip == NULL || port == NULL ||
									id == NULL){
		#if DEBUG == 1
			fprintf(stderr, "pushConfigFile :: Argument Error\n");
		#endif
		return ERR;
	}

	newid = (char *)malloc(strlen(id) + 2);
	strcpy(newid, id);
	strcat(newid, "\n");
	
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(port));
	inet_aton(ip, &address.sin_addr);

	sd = socket(AF_INET, SOCK_STREAM, 0);

	/* Try establishing a connection to the controller. */
	if(connect(sd, (struct sockaddr*)&address,
					 sizeof(struct sockaddr_in)) < 0){

		#if DEBUG == 1
			fprintf(stderr, "pushConfigFile :: %s\n", strerror(errno));
		#endif
		return ERR;
	}

	if((fd = open(filename, O_RDONLY)) < 0){
		#if DEBUG == 1
			fprintf(stderr, "pushConfigFile :: %s\n", strerror(errno));
		#endif
		return ERR;	
	}

	data = (uint8_t *)malloc(1024);		
	offset = 0;
	while((count = read(fd, data + offset, 1024)) > 0){
		offset += count;
		data = (uint8_t *)realloc(data, 1024);
	}
	close(fd);
	
	/* Send this over to be turned into a packet. */
	packet = newPacket(type, data, offset);		
	free(data);
	
	write(sd, newid, strlen(newid));

	/* Write the packet after this to the socket. */
	write(sd, packet, ((struct Packet *)packet)->len);

	close(sd);
	free(packet);
	free(newid);

	return 0;
}
