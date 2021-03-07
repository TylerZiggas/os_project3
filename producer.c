#include "monitor.h"

int main(int argc, char *argv[]) {
	srand(time(NULL));
	int i = (rand() % 300);
	// Log
	// store 
	sleep((rand() % (5 - 1 + 1)) + 1);
	printf("Producer produced %d\n", i);
	return 0;
}
