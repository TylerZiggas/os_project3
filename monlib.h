#pragma once
#ifndef MONLIB_H
#define MONLIB_H // Define this file

void produce(int); // Declare functions used in shared
void consume(int);
void spawnProducer(int, int);
void spawnConsumer(int, int);

#endif
