#pragma once
#ifndef MONITOR_H
#define MONITOR_H // Define this file

#include <ctype.h> // Declared headers for all files 
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>

#define FORMATTED_TIME_SIZE 50 // Time declarations
#define FORMATTED_TIME_FORMAT "%H:%M:%S"

struct SharedMemory {
	size_t maxPro;
	size_t maxCon;
	size_t total; // Total processes
	char logfile[20];
	int producerCounter;
	int consumerCounter;
	int monitorCounter;
	int item;
	pid_t parentid;
	pid_t pgid; // group pid
};

char* logfilename; // Passing of the file we will write to

int smKey; // Shared memory and semaphore declarations
int smID;
struct SharedMemory* sm;
sem_t mutex, empty, full;

void createFile(char*); // Functions declarations for shared.c
void logOutput(char*, char*, ...);
void allocateSM();
void attachSM();
void releaseSM();
void deleteSM();
void removeSM();
void sigact(int, void(int));
void signalHandler(int);
char* getFormattedTime();

#endif
