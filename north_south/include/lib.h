#ifndef LIB_H

#define LIB_H

#define READ_CHUNCK 64

#define CONTROLLER_NORTH_PORT "6665"
#define CONTROLLER_NORTH_IP "127.0.0.1"

uint8_t* newPacket(int type, uint8_t* data, int len);
int listenPackets(int fd);
char *libDbPut(char *table_name, char *key, 
						char *value, char *ip, char *port);
char *libDbGet(char *table_name, char *key, char *ip, char *port);

#endif
