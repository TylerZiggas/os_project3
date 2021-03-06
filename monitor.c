#include "monitor.h"

void setupTimer(int);
void spawnChild(int, int);
void signalHandler(int);
void helpMenu();

bool flag = false;

int main (int argc, char *argv[]) {
	int character, optargCount, maxProducers = 2, maxConsumers = 6,  timeSec = 100, items = 0; // Intiailize necessary variables
	char* logfile;
	bool allDigit = true;
	signal(SIGINT, signalHandler);

	while ((character = getopt(argc, argv, "o:p:c:t:h")) != -1) { // Set up command line parsing
		switch (character) { 
			case 'o': // Change the maxChildren at a time
                                logfile = optarg;
				createFile(logfile);
                                //} else {
                                 //       errno = 22;
                                  //      perror("-s requires an argument");
                                   //     helpMenu();
                                  //      return EXIT_FAILURE;
                                //}
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

	sm->maxPro = maxProducers;
	sm->maxCon = maxConsumers;
	sm->total = maxAll;
	pthread_t thread_pro[maxProducers];
	pthread_t thread_con[maxConsumers];
	
	int i;
	for (i = 0; i < maxProducers; i++) {
		printf("Produced producer %d\n", i);
		pthread_create(&thread_pro[i], NULL, produce, NULL);
	}
	for (i = 0; i < maxConsumers; i++) {
		printf("produced consumer\n");
		pthread_create(&thread_con[i], NULL, consume, NULL);
	}
	for (i = 0; i < maxProducers; i++) {
        	printf("Producer joining\n");
	        pthread_join(i, NULL);
        }

        for (i = 0; i < maxConsumers; i++) {
		printf("Consumer joining\n");	 
	        pthread_join(i, NULL);
        }

//	sem_destroy(&mutex);	
//	int childCounter = 0;
//	while (i < items) { // Go through until we are passed the number of items
//		if (childCounter < maxAll) { // Spawn children based on max allowed
//			spawnChild(childCounter++, i);
//			i++;
//		} else { // Else wait for children to finish and keep making more
//			while(wait(NULL) > 0);
//			childCounter = 0;
//		}
//	}
//	while(wait(NULL) > 0); // Wait for all to end before going to next depth
//	childCounter = 0;

	removeSM(); // Removing the shared memory once children are done
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

//void spawnChild(int childCounter, int i) { // Creation of children
//	pid_t pid = fork(); // Fork child process
//
//	if (pid == -1) { // Check to see if child process was created
//		perror("Failed to create a child process for bin_adder");
//		exit(EXIT_FAILURE);
//	}
//
//	if (pid == 0) { // If it was created successfully
//		flag = true;
//		
//		if (childCounter == 0) { // Set first as the group pid
//			sm->pgid = getpid();
//		}
//
//		setpgid(0, sm->pgid);
//		flag = false;
//		
//		char id[256], bufferi[3];	// Creation of id through childcounter, and casting i and depth into chars
//		sprintf(id, "%d", childCounter);	
//		sprintf(bufferi, "%d", i);
//		//execl("./bin_adder", "bin_adder", bufferi, bufferd, id, (char*) NULL); // Sending all information to bin_adder
//		exit(EXIT_SUCCESS);
//	}
//}

void signalHandler(int s) { // Signal handler for master
	if (flag) { // In case flag is currently working
		sleep(1);
	}

	killpg(sm->pgid, s == SIGALRM ? SIGUSR1 : SIGTERM);

	while (wait(NULL) > 0); // Wait for all child processes to finish

	printf("Parent exiting...\n");	
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
