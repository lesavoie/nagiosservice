/* Author : Nagios Service Team. 
 * Each packet contains a command, the interpreter
 * contains logic to parse these commands and call
 * the appropriate handlers attached to them.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <connection.h>
#include <interpreter.h>
#include <merrors.h>
#include <log.h>

static int configHost(uint8_t *buf, int size);
static int configHello(uint8_t *buf, int size);

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

	#if DEBUG == 1
		fprintf(stderr, "configHost :: Request host configuration\n");
	#endif

	return  0;
}

static int configHello(uint8_t *buf, int size){

	#if DEBUG == 1
		fprintf(stderr, "configHello :: Request hello packet\n");
	#endif

	return 0;
}
