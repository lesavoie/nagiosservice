#ifndef INTERPRETER_H

#define INTERPRETER_H

/* Command definations. */
#define HOST 1
#define SERVICE 2
#define CONTACT 3
#define HELLO 4
#define IDENT 5

/* Function prototypes. */
int parsePacket(uint8_t *buf);

#endif
