#include "monitor.h"

//void signalHandler(int);

int main (int argc, char *argv[]) {
	sem_t *mutex = sem_open("mutex", 1);
        sem_t *empty = sem_open("empty", 1);
        sem_t *full = sem_open("full", 0);
	sigact(SIGTERM, signalHandler);
	sigact(SIGUSR1, signalHandler);
	attachSM();
	// grab item
	// log
	// delete item
	sleep((rand() % (5 - 1 + 1)) + 1);
	printf("Consumer consumed item: \n");
	if (sm->monitorCounter == sm->maxPro-1) {
		sem_post(mutex);
		sem_post(empty);
	}
	sm->consumerCounter--;
	sm->monitorCounter--;
	return 0;
}

//void signalHandler(int s) { // Catches signal for ^C or Timeout
//	if (s == SIGTERM || s == SIGUSR1) {
//		printf("Consumer exiting... 2\n");
//		exit(EXIT_FAILURE);
//	}
//}
