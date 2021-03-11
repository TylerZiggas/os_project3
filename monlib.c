#include "monitor.h"
#include "monlib.h"

bool firstForGroup = true;

void produce(int producer) { // Monitor portion for multiple producers
	sem_t *mutex = sem_open("mutex", 0); // Opening semaphores
	sem_t *empty = sem_open("empty" , 0);
	sem_wait(empty); // Wait on these calls
	sem_wait(mutex);
	
	char id[256]; // Cast producer to char to pass it
	sprintf(id, "%d", producer);
	execl("./producer", id, NULL); // Rest of monitor in producer.c as execl never returns here
}

void consume(int consumer) { // Monitor protion for multiple consumers
        sem_t *mutex = sem_open("mutex", 0); // Opening semaphores
        sem_t *full = sem_open("full" , 0);
	sem_wait(full); 
	sem_wait(mutex);

	char id[256]; // Cast consumer to char to pass it
	sprintf(id, "%d", consumer);
	execl("./consumer", id, NULL); // Rest of monitor in consumer.c as execl never returns here	
}

void spawnProducer(int producer, int i) {
	pid_t pid = fork();
	if (pid == 0 && i == 0 && firstForGroup == true) { // Creation of group pid
		firstForGroup = false;
		sm->pgid = getpid();
	}
	setpgid(pid, sm->pgid);
	if (pid == 0) { // Make sure this is a child process
		sigact(SIGUSR1, signalHandler);
		produce(producer);
		exit(EXIT_SUCCESS);
	}
}

void spawnConsumer(int consumer, int i) {
	pid_t pid = fork();
	while (pid == 0 && !firstForGroup) {
		sleep(0);
	}
	setpgid(pid, sm->pgid);
	if (pid == 0) { // Make sure this is a child process
		sigact(SIGUSR1, signalHandler);
		consume(consumer);
		exit(EXIT_SUCCESS);
	}
}
