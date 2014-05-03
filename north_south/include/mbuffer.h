#ifndef MBUFFER_H

#define MBUFFER_H

struct Element{
	uint8_t *data;
	int size;
	struct Element *next;
};

struct Queue{
	struct Element *head;
	struct Element *tail;
};

/* Function prototype. */
void QInit();
int QAddData(uint8_t *data, int size);
void QStartSpool(int *fd);

#endif
