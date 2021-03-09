#include "monitor.h"

//void signalHandler(int);

int main (int argc, char *argv[]) {
	sem_t *mutex = sem_open("mutex", 0);
        sem_t *empty = sem_open("empty", 0);
        //sem_t *full = sem_open("full", 0);
	sigact(SIGTERM, signalHandler);
	sigact(SIGUSR1, signalHandler);
	attachSM();
	// grab item
	// log
	// delete item
	sleep((rand() % (10 - 1 + 1)) + 1);
	int i = sm->item; 
	printf("Consumer consumed item: %d\n", sm->item);

	sm->monitorCounter--;
	//if (sm->monitorCounter == sm->maxPro-1) {
		sem_post(mutex);
		sem_post(empty);
	//}
	sm->consumerCounter--;
	//sm->monitorCounter--;
	return 0;
}

//void signalHandler(int s) { // Catches signal for ^C or Timeout
//	if (s == SIGTERM || s == SIGUSR1) {
//		printf("Consumer exiting... 2\n");
//		exit(EXIT_FAILURE);
//	}
//}
