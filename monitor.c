// Author: Tyler Ziggas
// Date: March 2021
// This takes in the options from command line and applies/verifies the contents given, and then allocates shared memory.
// We then enter an infinite loop for creation of producers and consumers

#include "monitor.h"

void setupTimer(int);
void spawnChild(int, int);
void helpMenu();

int main (int argc, char *argv[]) {
	int character, optargCount, maxProducers = 2, maxConsumers = 6,  timeSec = 100; // Intiailize necessary variables
	char* logfile;
	bool allDigit = true, fileCreated = false;
	sigact(SIGINT, signalHandler);

	while ((character = getopt(argc, argv, "o:p:c:t:h")) != -1) { // Set up command line parsing
		switch (character) { 
			case 'o': // Creation of a logfile defined by user
				logfile = optarg;
				createFile(logfile);
				fileCreated = true;
                                continue;
			case 'p': // Change the max producers allowed
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
			 case 'c': // Change the max consumers allowed
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
			default: // Simply show help menu and exit if user inputs something that isnt allowed
				printf("Input not valid\n");
				helpMenu();
				return EXIT_FAILURE;
		}
	}
	
	if (!fileCreated) { // If file was not specified, make a default one
		logfile = "logfile";
		createFile(logfile);
	}

	allocateSM(); // Allocate memory for the whole program
	logOutput(logfile, "Time:%s | Allocated Shared Memory\n", getFormattedTime());
	sm->parentid = getpid();	
	if (maxProducers >= maxConsumers) { // Make sure that consumers is more than producers
		maxConsumers = maxProducers + 1;
	}
	int maxAll = maxProducers + maxConsumers;

	if (maxAll > 20) { // If the max of all is 20, just make it default and loop with these numbers
		maxProducers = 2;
		maxConsumers = 6;
		maxAll = maxProducers + maxConsumers;
	}
	
	strcpy(sm->logfile, logfile); // Copy filename to memory so we can write to this new logfile
	
	sm->maxPro = maxProducers; // Setting shared memory variables that are necessary for other parts 
	sm->maxCon = maxConsumers;
	sm->total = maxAll;
	sm->producerCounter = 0; // Initialize those variables before we start
	sm->consumerCounter = 0;
	sm->monitorCounter = 0;

	int i = 0;

	setupTimer(timeSec); // Setting up the timer
	
	logOutput(logfile, "Time:%s | Alarm set to end in %d seconds \n", getFormattedTime(), timeSec);
	sem_t *mutex = sem_open("mutex", O_CREAT, 0600, 1);
	sem_t *empty = sem_open("empty", O_CREAT, 0600, 1);
	sem_t *full = sem_open("full", O_CREAT, 0600, 0);
	
	sem_close(mutex); // Close them immediately as we won't use them in the main function
	sem_close(empty);
	sem_close(full);
	
	while (true) { // Go through until we are passed the number of items
		if (sm->producerCounter < maxProducers) { // Creation of producers
			spawnProducer(sm->producerCounter++, i);
			i++;
		} 
		if (sm->consumerCounter < maxConsumers) { // Creation of consumers
			spawnConsumer(sm->consumerCounter++, i);
			i++;
		} 
		if (!(sm->consumerCounter < maxConsumers) && !(sm->producerCounter < maxProducers)) { // Wait for a consumer or producer to end, will check itself to see which to make 
			wait(NULL);
		}
	}
	
	while(wait(NULL) > 0); // Wait for all to end

	removeSM(); // Removing the shared memory once children are done
	logOutput(logfile, "Time:%s | Deallocated Shared Memory\n", getFormattedTime());
	sem_unlink("full"); // Unlink our named semaphores in case something weird happens and we leave the infinite loop
	sem_unlink("empty");
	sem_unlink("mutex");
	
	return EXIT_SUCCESS;
}

void setupTimer(const int t) { // Creation of the timer
	sigact(SIGALRM, signalHandler); 
	sigact(SIGUSR1, signalHandler);

	struct itimerval timer; // Creation of the struct for timer
	timer.it_value.tv_sec = t;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	
	if (setitimer(ITIMER_REAL, &timer, NULL) == -1) { // In case of failed creation of the timer itself
		perror("Failed to set timer");
		exit(EXIT_FAILURE);
	}
}

void helpMenu() { // Help menu
	printf("This program taks a number of producers and consumers and ends at whatever the user wishes by setting the time.\n");
	printf("The following are commands after inputting ./monitor: -p *number* to specify the number of producers (default is 2)\n");
	printf("-c *number* to specify the number of consumers (default is 6)\n");
	printf("-t *number* to specify the amount of time to wait before termination (default is 100)\n");
	printf("-o *name* to create a file where the information will be logged (default is logfile)\n");
}
