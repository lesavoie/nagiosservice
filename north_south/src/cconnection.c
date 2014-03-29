/* Author : Nagios Service Team. */
/* Connection code which binds a connection, 
 * sends / receives data,
 * sends data to the monitor. 
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

#include <connection.h>
#include <cconnection.h>
#include <interpreter.h>
#include <merrors.h>
#include <log.h>

static int controller_CompChecksum(uint8_t *buf, uint32_t checksum);
static int controller_Handshake(int fd);
static void inline controller_CrtPacket(struct Packet *header, uint8_t type);

/* Function which binds a connection to the controller. 
 * Arg0 : IP address of the control server.
 * Arg1 : Port number for the control server on which to connect.
 * Returns : On SUCCESS returns the connection object, on 
 * FAILURE returns NULL;
 */
static struct Connection* controller_Bind(char *ip, char *port) {
	struct Connection *connection;


	if(ip == NULL || port == NULL){
		#if DEBUG == 1
			fprintf(stderr, "controller_Bind : Argument Error\n");
		#endif
		goto ret;
	}

	/* Create connection object. */					

	connection = (struct Connection *)
						malloc(sizeof(struct Connection));

	if(connection == NULL)
		goto ret;

	connection->address.sin_family = AF_INET;
	connection->address.sin_port = htons(atoi(port));
	inet_aton(ip, &connection->address.sin_addr);
	
	connection->msocket = socket(AF_INET, SOCK_STREAM, 0);	

	/* Try bindinf a conection at the controller */ 
	if(bind(connection->msocket, (struct sockaddr*)&(connection->address),
					 sizeof(struct sockaddr_in)) < 0){
		#if DEBUG == 1
			fprintf(stderr, "controller_Bind :: %s\n", strerror(errno));
		#endif	
		goto fret;
	}

	return connection;

fret:
	free(connection);

ret:
	return NULL;
}

/* Function which listens on the socket to see if there are any connections
 * from the monitoring servers.
 * Arg0 : Connection object.
 * Returns : This function ideally never returns, if it does 
 * return then there was some connection error. 
 */
static int controller_Loop(struct Connection *connection){
	
	int fd = -1;

	if(connection == NULL)
		goto ret;

	if(listen(connection->msocket, MAX_PENDING) < 0)
		goto ret;
	
	while((fd = accept(connection->msocket,
							NULL, NULL)) >= 0){
		/* A monitor has contacted us. */
		#if DEBUG == 1
			fprintf(stderr, "controller_loop :: Accepted monitor\n");
		#endif

		controller_Handshake(fd);
	}

ret:
	return CONERR; 
}

/* Do the initialization handshake. */
static int controller_Handshake(int fd){
	struct Packet header;
	uint8_t *data = NULL;
	int count = -1;

	if(fd < 0){
		#if DEBUG == 1
			fprintf(stderr, "controller_Handshake :: Invalid fd\n");
		#endif
		return ERR;
	}

	/* Create a hello packet. */
	controller_CrtPacket(&header, HELLO);
	header.len = sizeof(struct Packet) + DUMY;

	/* Add dumy data after this header so the monitor code 
 	 * does not cry about it. */
	data = (uint8_t *)malloc(sizeof(struct Packet)+DUMY);
	memset(data + sizeof(header), 0x0, DUMY);
	memcpy(data, &header, sizeof(struct Packet));

	/* Send the hello packet to the monitor. */
	if((count = write(fd, data, sizeof(struct Packet)+DUMY)) < 0){
		#if DEBUG == 1
			fprintf(stderr, "controller_Handshake :: write() fail\n");
		#endif 
	}

	free(data);

	return 0;
}

static void inline controller_CrtPacket(struct Packet *header, uint8_t type){
	header->magic_number = MAGIC_NUMBER;
	header->type = type;
	header->len = 0x0;
	header->checksum = 0x0;
}

/* Function which calculates the checksum for the given data
 * and then compares it with the passed checksum to see if they
 * match. For now this has been stubbed out to return a true. 
 * Arg0 : Data to calculate the checksum on.
 * Arg1 : Checksum to compare with.
 */
static int controller_CompChecksum(uint8_t *buf, uint32_t checksum){
	return 1;
}

/* Wrapper function which the daemon calls.
 * This function so ideally never return. */
void controller_StartComm(char *ip, char *port) {
	struct Connection *connection;

	connection = controller_Bind(ip, port);

	controller_Loop(connection);
}
