#include "monitor.h"

//void signalHandler(int);

int main(int argc, char *argv[]) {
	sem_t *mutex = sem_open("mutex", 1);
	sem_t *empty = sem_open("empty", 1);
	sem_t *full = sem_open("full", 0);
	sigact(SIGTERM, signalHandler); // Set up signals 
	sigact(SIGUSR1, signalHandler);
	attachSM();
	srand(time(NULL));
	int i = (rand() % 300);
	//printf("%s\n", logfilename);
	//printf("%s <- should be here", sm->logfile);
	//logOutput(sm->logfile, "Time%s | Producer created %d", getFormattedTime(), i);
	// store 
	sleep((rand() % (5 - 1 + 1)) + 1);
	printf("Producer produced item :%d\n", i);
	sm->item = i;
	sm->monitorCounter++;
	if (sm->monitorCounter == 1) {
		sem_post(mutex);
		sem_post(full);
	}
	sm->producerCounter--;
	//sm->monitorCounter++;
	return 0;
}

//void signalHandler(int s) { // Catches signal for ^C or Timeout
//	if (s == SIGTERM || s == SIGUSR1) {
//		printf("Producer exiting... 1\n");
//		exit(EXIT_FAILURE);
//	}
//}
