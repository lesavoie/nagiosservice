/* Authors: Nagios Service Team. */
/* A simple producer service which writes contents from the 
 * buffer to the network stream. This works as a spool,
 * as long as there is data in it, it will keep adding it 
 * to the network stream. */

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#include <mbuffer.h>
#include <merrors.h>

static struct Queue queue;
static pthread_mutex_t mutex;

static void *QSpoolData(void *args);

/* Function to init our buffer. */
void QInit(){

	/* Init the mutex. */
	pthread_mutex_init(&mutex, NULL);

	queue.head = NULL;
	queue.tail = NULL;

	return;
}

/* Function to add contents to the Queue. */
int QAddData(uint8_t *data, int size){
	struct Element *element;

	element = (struct Element*)
								malloc(sizeof(struct Element));

	if(element == NULL){
		#if DEBUG == 1
			fprintf(stderr, "QAddData ::malloc() error.\n");
		#endif
		return ERR;
	}

	pthread_mutex_lock(&mutex);

	element->data = data;
	element->size = size;
	element->next = NULL;
	if(queue.tail != NULL)
		queue.tail->next = element;
	queue.tail = element;

	/* If this was the only element added then adjust the head too. */
	if(queue.head == NULL)
		queue.head = queue.tail;

	pthread_mutex_unlock(&mutex);

	return 0;
}

/* Function which initiates the spool. */
void QStartSpool(int fd){
	pthread_t thread;	

	pthread_create(&thread, NULL, QSpoolData, (void*)&fd);
}

/* A tight loop which continously keeps throwing data into the
 * network stream. */
static void *QSpoolData(void *args){
	struct Element *element;
	int fd;
	
	fd = *((int*)args);

	if(fd < 0){
		#if DEBUG == 1
			fprintf(stderr, "QSpoolData :: Argument error\n");
		#endif

		pthread_exit(NULL);
	}

	while(1){
		/* A simple tight loop, which stalls till data is present. */
		while(queue.head == NULL);

		pthread_mutex_lock(&mutex);

		/* If there is some data present throw it into the network stream. */
		element = queue.head;
		if(write(fd, element->data, element->size) < 0){
			#if DEBUG == 1	
				fprintf(stderr, "QSpoolData :: write() error\n");
			#endif
		}

		/* Shift the head. */
		queue.head = element->next;
		if(queue.head == NULL)
			queue.tail = NULL;

		pthread_mutex_unlock(&mutex);
		
		free(element->data);
		free(element);
	}
}
