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
#include <pthread.h>

#include <connection.h>
#include <cconnection.h>
#include <interpreter.h>
#include <merrors.h>
#include <lib.h>
#include <log.h>

#include <cfuhash.h>

static int controller_CompChecksum(uint8_t *buf, uint32_t checksum);
static int controller_Handshake(int fd);
static void inline controller_CrtPacket(struct Packet *header, uint8_t type);
static void *controller_wrpHandshake(void *args);
static void *controller_SouthLoop(void *args);
static void *controller_NorthLoop(void *args);
static void *controller_wrpNorth(void *args);

/* A global hash object which holds a mapping of monitor id
 * and fd it belongs to. */
cfuhash_table_t *controller_map;

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
static void *controller_SouthLoop(void *args){
	int fd = -1;
	pthread_t *thread = NULL;
	struct Connection *connection = NULL;

	connection = (struct Connection *)args;

	if(connection == NULL)
		goto ret;

	if(listen(connection->msocket, MAX_PENDING) < 0)
		goto ret;
	
	while((fd = accept(connection->msocket,
							NULL, NULL)) >= 0){
		/* A monitor has contacted us. */
		#if DEBUG == 1
			fprintf(stderr, "controller_SouthLoop :: Accepted monitor\n");
		#endif

		/* FIXME: Memory leak. */
		thread = (pthread_t *)malloc(sizeof(pthread_t));
		pthread_create(thread, NULL, controller_wrpHandshake, (void*)&fd);	
	}

ret:
	pthread_exit(NULL); 
}

/* Function which listens on the socket to see if there are any connections
 * from the north bound to accept configuration files.
 * Arg0 : Connection object.
 * Returns : This function ideally never returns, if it does 
 * return then there was some connection error. 
 */
static void *controller_NorthLoop(void *args){
	int fd = -1;
	pthread_t *thread = NULL;
	struct Connection *connection = NULL;

	connection = (struct Connection *)args;

	if(connection == NULL)
		goto ret;

	if(listen(connection->msocket, MAX_PENDING) < 0)
		goto ret;
	
	while((fd = accept(connection->msocket,
							NULL, NULL)) >= 0){
		/* A monitor has contacted us. */
		#if DEBUG == 1
			fprintf(stderr, "controller_NorthLoop :: Accepted connection\n");
		#endif

		/* FIXME: Memory leak. */
		thread = (pthread_t *)malloc(sizeof(pthread_t));
		pthread_create(thread, NULL, controller_wrpNorth, (void*)&fd);	
	}

ret:
	pthread_exit(NULL);
}

/* Function which deals with taking data from the north bound
 * and pushing it onto the south bound. */
static void *controller_wrpNorth(void *args){
	int fd = -1;
	int count = -1;
	char temp;
	char *key;
	int index = 0;
	int *south_fd = NULL;
	uint8_t *forward_buffer;
	struct Packet packet;	

	fd = *((int*)args);
	key = (char*)malloc(sizeof(char) * 256);

	/* Read the monitor id from the north bound. */
	while((count = read(fd, &temp, sizeof(char))) > 0){
		if(temp == '\n' || temp == '\r'){
			key[index] = '\0';
			break;
		}
		key[index++] = temp;
	}

	/* Search for the key in the hash table. 
	 * If the key is not found close the connection and return. */
	south_fd = (int*)cfuhash_get(controller_map, key);	

	#if DEBUG == 1
		fprintf(stderr, "controller_wrpNorth :: key => %s\n", key);	
	#endif

	if(south_fd == NULL){
		#if DEBUG == 1
			fprintf(stderr, "controller_wrpNorth :: Hash map miss\n");
			cfuhash_pretty_print(controller_map, stderr);
		#endif
		goto ret;
	}

	#if DEBUG == 1
		fprintf(stderr, "controller_wrpNorth :: south fd => %d\n", *south_fd);
	#endif

	/* Read contents from the north bound interface and pass them
	 * on the correct connection stream. */
	/* Read the packet header. */

	count = read(fd, &packet, sizeof(struct Packet));
	
	forward_buffer = (uint8_t*)malloc(packet.len);
	memcpy(forward_buffer, &packet, sizeof(struct Packet));
	
	count = read(fd, forward_buffer + sizeof(struct Packet), 
				packet.len - sizeof(struct Packet));

	count = write(*south_fd, forward_buffer, packet.len);
	free(forward_buffer);

ret:
	close(fd);
	free(key);
	pthread_exit(NULL);
}

/* A wrapper function which initiates the handshake protocol,
 * and starts a listening service at the socket.
 */
static void *controller_wrpHandshake(void *args){
	int fd = -1;

	fd = *((int*)args);

	/* Start a service which listens onto the socket for 
	 * incoming messages. */
	controller_Handshake(fd);
	listenPackets(fd);
	close(fd);

	pthread_exit(NULL);
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
	struct Connection *connection_south;
	struct Connection *connection_north;
	pthread_t thread_north, thread_south;

	/* Initialize the controller map. */
	controller_map = cfuhash_new_with_initial_size(BUCKET_NUM);

	connection_south = controller_Bind(ip, port);
	connection_north = controller_Bind(CONTROLLER_NORTH_IP,
											CONTROLLER_NORTH_PORT);

	/* Create 2 threads, one to listen north bound & the other
	 * to listen south bound. */
	pthread_create(&thread_north, NULL, controller_NorthLoop,
						(void*)connection_north);
	pthread_create(&thread_south, NULL, controller_SouthLoop,
						(void*)connection_south);

	pthread_join(thread_south, NULL);
	pthread_join(thread_north, NULL);
}
