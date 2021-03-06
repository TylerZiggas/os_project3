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
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define FORMATTED_TIME_SIZE 50 
#define FORMATTED_TIME_FORMAT "%H:%M:%S"

#define THINKING 2
#define HUNGRY 1
#define EATING 0

struct SharedMemory {
	size_t total; // Total processes
	pid_t pgid; // group pid
};

int smKey;
int smID;
struct SharedMemory* sm;

key_t semKey;
int semId;

void createFile(char*);
void logOutput(char*, char*, ...);
void allocateSM();
void attachSM();
void releaseSM();
void deleteSM();
void removeSM();
void semAllocate(bool);
void semRelease();
char* getFormattedTime();

#endif
