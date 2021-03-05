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
