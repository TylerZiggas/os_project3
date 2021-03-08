#include "monitor.h"

void createFile(char* path) { // Creation of files
	FILE* newFile = fopen(path, "w"); // Take whatever char* was passed
		if (newFile == NULL) { // If there is a problem creating the file
			perror("Failed to touch file");
			exit(EXIT_FAILURE);
		}
	fclose(newFile); // Close file at end regardless
}

void logOutput(char* path, char* fmt, ...) {
	FILE* fp = fopen(path, "a+"); // Open a file for writing
	
	if (fp == NULL) { // In case of failed logging to file
		perror("Failed to open file for logging output");
		exit(EXIT_FAILURE);
	}
		
	int n = 4096; 
	char buf[n];
	va_list args; // Intialize to grab all arguments for logging
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);	
	fprintf(fp, buf); // Writing to the file 
	fclose(fp); 
}

void allocateSM() { // Attaches the shared memory to whatever calls it
	attachSM();
	releaseSM();
	attachSM();
}

void removeSM() { // Removes the shared memory from whatever calls it and deletes it
	releaseSM();
	deleteSM();
}

void attachSM() { // Creation of shared memory
	smKey = ftok("Makefile", 'p'); // Use makefile to create key
	if ((smID = shmget(smKey, sizeof(struct SharedMemory), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) { // Make sure memory was allocated
		perror("Failed to allocate shared memory for SM");
		exit(EXIT_FAILURE);
	} else { 
		sm = (struct SharedMemory*) shmat(smID, NULL, 0);
	}
}

void releaseSM() { // Relaseing the memory
	if (sm != NULL) { 
		if (shmdt(sm)) { // Check if we fail to do so
			perror("Failed to release SM");
			exit(EXIT_FAILURE);
		}
	}
}

void deleteSM() { // Deletion of memory
	if (smID > 0) {
		if (shmctl(smID, IPC_RMID, NULL) < 0) { 
			perror("Failed to delete SM");
			exit(EXIT_FAILURE);
		}
	}
}

char* getFormattedTime() { // Creation of formatted time, mostly for log file
        char* formattedTime = malloc(FORMATTED_TIME_SIZE * sizeof(char)); // allocate memory for it
        time_t now = time(NULL);
        strftime(formattedTime, FORMATTED_TIME_SIZE, FORMATTED_TIME_FORMAT, localtime(&now)); // format time we just recieved
        return formattedTime;
}

void sigact(int signum, void handler(int)) {
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		perror("Sig set error");
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	if (sigaction(signum, &sa, NULL) == -1) {
		perror("Sigaction error");
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int s) {
	sem_unlink("mutex");
	sem_unlink("empty");
	sem_unlink("full");
	if (s == SIGTERM || s == SIGUSR1) {
		printf("Child exiting...\n");
		exit(EXIT_FAILURE);
	}

	killpg(sm->pgid, s == SIGALRM ? SIGUSR1 : SIGTERM);
	while (wait(NULL) > 0);
	sleep(1);
	removeSM();
	//sem_unlink("mutex");
	//sem_unlink("empty");
	//sem_unlink("full");
	printf("Monitor exiting...\n");
	exit(EXIT_SUCCESS);
}
//Monitor block

//int monitorCounter = 0;

void produce(int producer) {
	sigact(SIGTERM, signalHandler); // Set up signals 
	sigact(SIGUSR1, signalHandler);
	//printf("producer produce function%d\n", producer);
	sem_t *mutex = sem_open("mutex", 1);
	sem_t *empty = sem_open("empty" , 1);
	sem_t *full = sem_open("full" , 0);
	//sm->monitorCounter++;
	if (sm->monitorCounter == sm->maxPro) {
		//printf("Producer waiting on full\n");
		sem_wait(empty);
		sem_wait(mutex);
	}
	
	char id[256];
	sprintf(id, "%d", producer);
	execl("./producer", id, NULL);		
	//printf("Producer %d\n", producer);
	//monitorCounter++;
	//printf("%d\n", counter);
	
	//if (monitorCounter == 1) {
	//	sem_post(mutex);
	//	sem_post(full);		
	//}
	//sem_close(mutex);
	//sem_close(empty);
	//sem_close(full);
}

void consume(int consumer) {
	sigact(SIGTERM, signalHandler); // Set up signals 
	sigact(SIGUSR1, signalHandler);
	//printf("consumer consume function%d\n", consumer);
        sem_t *mutex = sem_open("mutex", 1);
        sem_t *empty = sem_open("empty" , 1);
        sem_t *full = sem_open("full" , 0);
	//sm->monitorCounter--;
	if (sm->monitorCounter == 0) {
		//printf("Consumer waiting on empty\n");
		sem_wait(full);
		//printf("Consumer past full\n");
		sem_wait(mutex);
		printf("Consumer past full and mutex\n");
	}

	char id[256];
	sprintf(id, "%d", consumer);
	execl("./consumer", id, NULL);	
	//printf("Consumer %d\n", consumer);
	//monitorCounter--;

	//if (monitorCounter == sm->maxPro-1) {
	//	sem_post(mutex);
	//	sem_post(empty);
	//}
	//sem_close(mutex);
	//sem_close(empty);
	//sem_close(full);
}

void spawnProducer(int producer, int i) {
	pid_t pid = fork();
	if (i == 0) {
		sm->pgid = getpid();
	}
 	setpgid(0, sm->pgid);
	if (pid == 0) {
		//while (true) {
			//printf("Producer %d here", i);
			produce(producer);
			exit(EXIT_SUCCESS);
		//}
		//char id[256];
		//sprintf(id, "%d", producer);
		//printf("producer %d\n", producer);
		//execl("./producer", id, NULL);
		//exit(EXIT_SUCCESS);
	}
}

void spawnConsumer(int consumer, int i) {
	pid_t pid = fork();
	//if (i == 0) {
	//	sm->pgid = getpid();
	//}
	//setpgid(0, sm->pgid);
	if (pid == 0) {
		//while (true) {
			//printf("Consumer %d here", i);
			consume(consumer);
			exit(EXIT_SUCCESS);
		//}
		//char id[256];
		//sprintf(id, "%d", consumer);
		//printf("consumer %d\n", consumer);
		//execl("./consumer", id, NULL);	
		//exit(EXIT_SUCCESS);
	}
}
//End of Monitor block
