// Author: Tyler Ziggas
// Date: March 2021
// Producer produces an item once the semaphores tell it that it is good to go, and opens it for consumers

#include "monitor.h"

int main(int argc, char *argv[]) {
	sem_t *mutex = sem_open("mutex", 0); // Open necessary semaphores for use after creation
	sem_t *full = sem_open("full", 0);
	attachSM(); // Attach memory to this child
	srand(time(NULL)); 
	int i = (rand() % 300); // Creation of a random item
	logOutput(sm->logfile, "Time %s | Producer created item %d\n", getFormattedTime(), i); // Log it
	sleep((rand() % (5 - 1 + 1)) + 1); // Sleep from 1-5 seconds
	printf("Producer produced item: %d\n", i);
	sm->item = i;
	sm->monitorCounter++;
	sem_post(mutex); // Signal that the consumer is good to go now that the item is made
	sem_post(full);
	sm->producerCounter--;
	return 0;
}
