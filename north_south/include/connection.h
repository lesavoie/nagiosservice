#ifndef CONNECTION_H

#define CONNECTION_H

struct Connection {
	/* Maintain the connection information in case
	 * we need to re-connect after a connection break. */
	struct sockaddr_in address;
	int msocket;
};

struct Packet {
	uint32_t magic_number;	/* Used to indicated packet start. */
	uint8_t type;
	uint16_t len;
	uint32_t checksum;
}__attribute((packed))__;

#define MAGIC_NUMBER 0xBAD2FACE

#endif
