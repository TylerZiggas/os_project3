GCC = gcc
CFLAGS = -Wall -g

MONITOR_DEP = monitor.c
MONITOR_OBJ = $(MONITOR_DEP:.c=.o)
MONITOR = monitor
PRO_DEP = producer.c
PRO_OBJ = $(PRO_DEP:.c=.o)
PRODUCER = producer
CON_DEP = consumer.c
CON_OBJ = $(CON_DEP:.c=.o)
CONSUMER = consumer
HEADER = monitor.h
OBJ = shared.o

OUTPUT = $(MONITOR) $(PRODUCER) $(CONSUMER)
all: $(OUTPUT)

$(MONITOR): $(MONITOR_OBJ) $(OBJ)
	$(GCC) $(CFLAGS) $(MONITOR_OBJ) $(OBJ) -o $(MONITOR)

$(PRODUCER): $(PRO_OBJ) $(OBJ)
	$(GCC) $(CFLAGS) $(PRO_OBJ) $(OBJ) -o $(PRODUCER)

$(CONSUMER): $(CON_OBJ) $(OBJ)
	$(GCC) $(CFLAGS) $(CON_OBJ) $(OBJ) -o $(CONSUMER)

%.o: %.c $(HEADER)
	$(GCC) $(CFLAGS) -c $*.c -o $*.o

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o  *_log

