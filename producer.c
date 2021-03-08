#include "monitor.h"

//void signalHandler(int);

int main(int argc, char *argv[]) {
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
	printf("Producer produced %d\n", i);
	return 0;
}

//void signalHandler(int s) { // Catches signal for ^C or Timeout
//	if (s == SIGTERM || s == SIGUSR1) {
//		printf("Producer exiting... 1\n");
//		exit(EXIT_FAILURE);
//	}
//}
