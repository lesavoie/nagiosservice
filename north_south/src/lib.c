/* Author: Nagios Service Team. */
/* Library function common to both the monitor
 * and the controller. */
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>

#include <interpreter.h>
#include <connection.h>
#include <lib.h>
#include <merrors.h>
#include <log.h>

static int CompChecksum(uint8_t *buf, uint32_t checksum);

/* Function to create a packet. */
uint8_t* newPacket(int type, uint8_t* data, int len){
	struct Packet header;
	uint8_t *buf;

	if(type < 0 || len < 0 || data == NULL){
		#if DEBUG == 1
			fprintf(stderr, "newPacket :: Argument Error\n");
		#endif

		return NULL;
	}

	header.magic_number = MAGIC_NUMBER;
	header.type = type;
	header.len = len + sizeof(struct Packet);
	header.checksum = 0x0;

	buf = (uint8_t*)malloc(sizeof(struct Packet) + len);	
	memcpy(buf, &header, sizeof(struct Packet));
	memcpy(buf+sizeof(struct Packet), data, len);

	/* Calculate the checksum and send the contents back. */
	return buf;
}

/* Function which sticks onto a socket and listens for incoming
 * packets. */
int listenPackets(int fd){
	struct Packet packet;
	int count = -1;
	int datalen = -1;
	uint8_t *data;
	/* A data blob used to store the complete packet. */
	uint8_t *buf;
	uint32_t checksum = 0x0;

	if(fd < 0){
		#if DEBUG == 1
			fprintf(stderr, "listenPackets :: Argument Error\n");
		#endif

		return ERR;
	}

	memset(&packet, 0x0, sizeof(struct Packet));

	while((count = read(fd, (void *)&packet,
							sizeof(struct Packet))) >= 0){
		
		#if DEBUG == 1 
			fprintf(stderr, "listenPackets :: Received packet\n"); 
		#endif

		/* Do some sanity check on the packet header. */
		if(count < sizeof(struct Packet) ||
						packet.magic_number != MAGIC_NUMBER){
			/* Err : Corrupted packet. */
			#if DEBUG == 1
				fprintf(stderr, "listenPackets :: Packet Corrupted\n");
			#endif
			continue;	
		}

		/* Read in the amount of data indicated by the header. */
		datalen = packet.len - sizeof(struct Packet);

		#if DEBUG == 1
			fprintf(stderr, "listenPackets :: Data Size %d\n", datalen);
		#endif
		
		if((data = (uint8_t *)malloc(datalen)) ==  NULL){
			/* Err : Memory error. */
			#if DEBUG == 1 
				fprintf(stderr, "listenPackets :: Memory Error\n");
			#endif
		}
		/* FIXME: A miscreated packet can potentially enable the read()
		 * to block indefinately. For instance: A packet who's
		 * len field indicates @sizeof(struct Packet) + 4, is purposfully
		 * wrongly crafted with no data in it, event though the header
		 * indicates 4. Hence a timeout based mechanism must be in place.
		 * There are a lot of other errors like, the following read() might
		 * be tricked to accept a packet header as data, however currently
		 * it is assumed that the libraries at the control server are
		 * trusted. */
		count = read(fd, (void*)data,
						datalen);	
	
		if((buf = (uint8_t *)malloc(packet.len)) == NULL){
			/* Err : Memory error. */
			continue;
		}

		/* Copy the header. */
		checksum = packet.checksum;
		packet.checksum = 0x0;
		memcpy(buf, &packet, sizeof(struct Packet));
		memset(&packet, 0x0, sizeof(struct Packet));
		
		/* Copy the data. */
		memcpy(buf+sizeof(struct Packet), data, datalen);
		free(data);

		/* Calculate a checksum over the complete packet for sanity
		 * check. */
		if(!CompChecksum(buf, checksum)){
			/* Err : Corrupted packet. */
		}

		/* Finally send this packet over to the interpreter,
		 * which semantically makes sense of the packet and 
		 * performs the required actions on it. */
		parsePacket(buf);
	}

	return ERR;
}

/* Function which calculates the checksum for the given data
 * and then compares it with the passed checksum to see if they
 * match. For now this has been stubbed out to return a true. 
 * Arg0 : Data to calculate the checksum on.
 * Arg1 : Checksum to compare with.
 */

static int CompChecksum(uint8_t *buf, uint32_t checksum){
	return 1;
}
