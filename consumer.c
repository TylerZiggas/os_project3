// Author: Tyler Ziggas
// Date: March 2021
// Consumer consumes an item that was put in shared memory by the producer when the producer allows the consumer to

#include "monitor.h"

//void signalHandler(int);

int main (int argc, char *argv[]) {
	sem_t *mutex = sem_open("mutex", 0); // Open necessary semaphores for use here
        sem_t *empty = sem_open("empty", 0);
	//sigact(SIGTERM, signalHandler); // Signals incase one is invoked while here
	sigact(SIGUSR1, signalHandler);
	attachSM(); // Attach memory to this child
	sleep((rand() % (10 - 1 + 1)) + 1); // Sleep from between 1-10 seconds
	int i = sm->item;
	logOutput(sm->logfile, "Time %s | Consumer removed %d\n", getFormattedTime(), i); // Log the item that was just grabbed
	printf("Consumer consumed item: %d\n", sm->item);
	sm->monitorCounter--;
	sem_post(mutex); // Free up the producers
	sem_post(empty);
	sm->consumerCounter--;
	return 0;
}

//void signalHandler(int s) { // Catches signal for ^C or Timeout
//	if (s == SIGTERM || s == SIGUSR1) {
//		printf("Consumer exiting... 2\n");
//		exit(EXIT_FAILURE);
//	}
//}
