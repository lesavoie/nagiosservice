/* Author : Nagios Service Team. */
/* Connection code which establishes a connection, 
 * sends / receives data,
 * sends the data to the interpreter,
 * sends acks back to the controller. */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>

#include <connection.h>
#include <mconnection.h>
#include <merrors.h>
#include <interpreter.h>
#include <mbuffer.h>
#include <lib.h>
#include <log.h>

static int monitor_CompChecksum(uint8_t *buf, uint32_t checksum);

/* Function which establishes a connection to the controller. 
 * Arg0 : IP address of the control server.
 * Arg1 : Port number for the control server on which to connect.
 * Returns : On SUCCESS returns the connection object, on 
 * FAILURE returns NULL;
 */
static struct Connection* monitor_Connect(char *ip, char *port) {
	struct Connection *connection;


	if(ip == NULL || port == NULL)
		goto ret;

	/* Create connection object. */					

	connection = (struct Connection *)
						malloc(sizeof(struct Connection));

	if(connection == NULL)
		goto ret;

	connection->address.sin_family = AF_INET;
	connection->address.sin_port = htons(atoi(port));
	inet_aton(ip, &connection->address.sin_addr);
	
	connection->msocket = socket(AF_INET, SOCK_STREAM, 0);	

	/* Try establishing a connection to the controller. */
	if(connect(connection->msocket, (struct sockaddr*)&connection->address,
					 sizeof(struct sockaddr_in)) < 0){

		#if DEBUG == 1
			fprintf(stderr, "monitor_Connect :: %s\n", strerror(errno));
		#endif
		goto fret;
	}

	return connection;

fret:
	free(connection);

ret:
	return NULL;
}

/* Function which listens on the socket to see if there are any instructions
 * from the control server coming to the monitoring server. 
 * Arg0 : Connection object.
 * Returns : This function ONLY returns when the connection to the
 * control server is borken. It is the responsibility of the calling
 * function to then re-establish connection or deal appropriately 
 * with the error. On connection break a CONERR is returned.
 */
static int monitor_Loop(struct Connection *connection){
	
	if(connection == NULL){
		return ERR;
	}

	return listenPackets(connection->msocket);
}

/* Function which calculates the checksum for the given data
 * and then compares it with the passed checksum to see if they
 * match. For now this has been stubbed out to return a true. 
 * Arg0 : Data to calculate the checksum on.
 * Arg1 : Checksum to compare with.
 */

static int monitor_CompChecksum(uint8_t *buf, uint32_t checksum){
	return 1;
}

/* Wrapper function which the daemon calls.
 * This function so ideally never return. */
void monitor_StartComm(char *ip, char *port) {
	struct Connection *connection;

	connection = monitor_Connect(ip, port);

	/* Initialize and the start the buffer which pushes contents
	 * onto the network stream. */
	QInit();
	QStartSpool(connection->msocket);
	
	monitor_Loop(connection);
}
