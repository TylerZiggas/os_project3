#include "monitor.h"

void signalHandler(int);

int main (int argc, char *argv[]) {
	signal(SIGTERM, signalHandler);
	signal(SIGUSR1, signalHandler);
	attachSM();
	// grab item
	// log
	// delete item
	sleep((rand() % (5 - 1 + 1)) + 1);
	printf("Consumer here\n");
	return 0;
}

void signalHandler(int s) { // Catches signal for ^C or Timeout
	if (s == SIGTERM || s == SIGUSR1) {
		printf("Consumer exiting... 2\n");
		exit(EXIT_FAILURE);
	}
}
