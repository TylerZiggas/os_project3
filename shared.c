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

void sigact(int signum, void handler(int)) { // Creation of signals for timer and ctrl c in various spots of the program
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		perror("Sig set error");
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	if (sigaction(signum, &sa, NULL) == -1) { // Making sure sigaction is set correctly
		perror("Sigaction error");
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int s) { 
	if (sm->parentid != getpid()){ // Check if you are not the parent, kill children
		printf("Child exiting...\n");
		exit(EXIT_FAILURE);
	}

	if (sm->parentid == getpid() && (s == SIGALRM || s == SIGUSR1)) { // Print if specifically time ran out
		printf("Alarm triggered, please wait while children end...\n");
	}	

        sem_unlink("mutex"); // Unlink the semaphores that were used
        sem_unlink("empty");
        sem_unlink("full");
	
	logOutput(sm->logfile, "Time:%s | Signal to end process has been caught\n", getFormattedTime()); // Log that the process is ending
	killpg(sm->pgid, s == SIGALRM ? SIGUSR1 : SIGINT); 
	while (wait(NULL) > 0); // Wait for children in case some are still waiting to die
        logOutput(sm->logfile, "Time:%s | Deallocated Shared Memory\n", getFormattedTime());
	removeSM(); // Remove shared memory
	printf("Monitor exiting...\n");
	exit(EXIT_SUCCESS);
}

//Monitor block

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
//End of Monitor block
