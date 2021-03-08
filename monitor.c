#include "monitor.h"

void setupTimer(int);
void spawnChild(int, int);
void signalHandler(int);
void helpMenu();

//bool flag = false;

int main (int argc, char *argv[]) {
	int character, optargCount, maxProducers = 2, maxConsumers = 6,  timeSec = 5; // Intiailize necessary variables
	char* logfile;
	bool allDigit = true, fileCreated = false;
	signal(SIGINT, signalHandler);

	while ((character = getopt(argc, argv, "o:p:c:t:h")) != -1) { // Set up command line parsing
		switch (character) { 
			case 'o': // Change the maxChildren at a time
				logfile = optarg;
				createFile(logfile);
				fileCreated = true;
                                continue;
			case 'p': // Change the maxChildren at a time
				allDigit = true;
				for (optargCount = 0; optargCount < strlen(optarg); optargCount++) { // Check if entire optarg is digit
					if (!isdigit(optarg[optargCount])) {
						allDigit = false;
						break;
					}
				}		
				if (allDigit) { // After that, check if it meets requirements to change max
					maxProducers = atoi(optarg);
				} else {
					errno = 22;
					perror("-s requires a valid argument");
					helpMenu();
					return EXIT_FAILURE;
				}
				continue;
			 case 'c': // Change the maxChildren at a time
                                allDigit = true;
                                for (optargCount = 0; optargCount < strlen(optarg); optargCount++) { // Check if entire optarg is digit
                                        if (!isdigit(optarg[optargCount])) {
                                                allDigit = false;
                                                break;
                                        }
                                }
                                if (allDigit) { // After that, check if it meets requirements to change max
                                         maxConsumers = atoi(optarg);
                                } else {
                                        errno = 22;
                                        perror("-s requires a valid argument");
                                        helpMenu();
                                        return EXIT_FAILURE;
                                }
                                continue;
			case 't': // Change the max allowed time
				allDigit = true;
				for (optargCount = 0; optargCount < strlen(optarg); optargCount++) { // Check if optarg is digit
					if(!isdigit(optarg[optargCount])) {
						allDigit = false;
						break;
					}
				}
				if (allDigit) { // See if we meet requirements to change timer
					if (atoi(optarg) > 0) {
						timeSec = atoi(optarg);
					}
				} else {
					errno = 22;
					perror("-t requires a valid argument");
					helpMenu();
					return EXIT_FAILURE;
				}
				continue;
			case 'h': // Show a help menu if confused
				helpMenu();
				continue;
			default: 
				printf("Input not valid\n");
				helpMenu();
				return EXIT_FAILURE;
		}
	}
	
	if (!fileCreated) {
		logfile = "logfile";
		createFile(logfile);
	}

	allocateSM(); // Allocate memory for the whole program
	//semAllocate(true);
	setupTimer(timeSec); // Set up the timer
	
	if (maxProducers >= maxConsumers) {
		maxConsumers = maxProducers + 1;
	}
	int maxAll = maxProducers + maxConsumers;

	if (maxAll > 20) {
		maxProducers = 2;
		maxConsumers = 6;
		maxAll = maxProducers + maxConsumers;
	}
	printf("%s\n", logfile);
	logfilename = logfile;
	sm->maxPro = maxProducers;
	sm->maxCon = maxConsumers;
	sm->total = maxAll;

	int producerCounter = 0;
	int consumerCounter = 0;
	int i = 0;
	sem_t *mutex = sem_open("mutex", O_CREAT, 0600, 0);
	sem_t *empty = sem_open("empty", O_CREAT, 0600, 0);
	sem_t *full = sem_open("full", O_CREAT, 0600, 0);
	sem_close(mutex);
	sem_close(empty);
	sem_close(full);
	while (i < maxAll) { // Go through until we are passed the number of items
		if (producerCounter < maxProducers) { // Spawn children based on max allowed
			//printf("Spawning producer...");
			spawnProducer(producerCounter++, i);
			i++;
		} //else { // Else wait for children to finish and keep making more
		//	while(wait(NULL) > 0);
		//	producerCounter = 0;
		//}

		if (consumerCounter < maxConsumers) {
			//printf("Spawning consumer...");
			spawnConsumer(consumerCounter++, i);
			i++;
		} //else {
		//	while(wait(NULL) > 0);
		//	consumerCounter = 0;
		//}
	}
	while(wait(NULL) > 0); // Wait for all to end before going to next depth
//	childCounter = 0;

	removeSM(); // Removing the shared memory once children are done
	sem_unlink("full");
	sem_unlink("empty");
	sem_unlink("mutex");
	
	//semRelease();
	return EXIT_SUCCESS;
}

void setupTimer(const int t) { // Creation of the timer
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = signalHandler;

	if (sigaction(SIGALRM, &action, NULL) != 0) { // In case of failed signal creation
		perror("Failed to set signal action for timer");
		exit(EXIT_FAILURE);	
	}

	struct itimerval timer;
	timer.it_value.tv_sec = t;
	timer.it_value.tv_usec = t;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	
	if (setitimer(ITIMER_REAL, &timer, NULL) != 0) { // In case of failed creation of the timer itself
		perror("Failed to set timer");
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int s) { // Signal handler for master
	killpg(sm->pgid, s == SIGALRM ? SIGUSR1 : SIGTERM);

	while (wait(NULL) > 0); // Wait for all child processes to finish

	printf("Monitor exiting...\n");	
	sem_unlink("mutex");
	sem_unlink("empty");
	sem_unlink("full");
	removeSM(); // Deallocate and destroy shared memory
	exit(EXIT_SUCCESS);
}

void helpMenu() { // Help menu
	printf("This program taks a number of producers and consumers and ends at whatever the user wishes by setting the time.\n");
	printf("The following are commands after inputting ./monitor: -p *number* to specify the number of producers (default is 2)\n");
	printf("-c *number* to specify the number of consumers (default is 6)\n");
	printf("-t *number* to specify the amount of time to wait before termination (default is 100)\n");
	printf("-o *name* to create a file where the information will be logged (default is logfile)\n");
}
