#ifndef LIB_H

#define LIB_H

uint8_t* newPacket(int type, uint8_t* data, int len);
int listenPackets(int fd);

#endif
