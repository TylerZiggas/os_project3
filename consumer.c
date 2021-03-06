// Author: Tyler Ziggas
// Date: March 2021
// Consumer consumes an item that was put in shared memory by the producer when the producer allows the consumer to

#include "monitor.h"
#include "monlib.h"

int main (int argc, char *argv[]) {
	sem_t *mutex = sem_open("mutex", 0); // Open necessary semaphores for use here
        sem_t *empty = sem_open("empty", 0);
	attachSM(); // Attach memory to this child
	printf("Consumer now sleeping from 1-10 seconds...\n");
	sleep((rand() % (10 - 1 + 1)) + 1); // Sleep from between 1-10 seconds
	int i = sm->item;
	logOutput(sm->logfile, "Time %s | Consumer removed %d\n", getFormattedTime(), i); // Log the item that was just grabbed
	printf("Consumer consumed item: %d\n", sm->item);
	sm->item = 0;
	sm->monitorCounter--;
	sem_post(mutex); // Free up the producers
	sem_post(empty);
	return 0;
}
