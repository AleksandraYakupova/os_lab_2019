CC=gcc
CFLAGS=-I.

all: parallel_min_max process_memory 

parallel_min_max: utils.o find_min_max.o utils.h find_min_max.h
    $(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)
    
utils.o : utils.h
    $(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
    $(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory: process_memory.c
    $(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_sum: sum_utils.h sum_utils.c utils.o sum_utils.so 
	gcc -pthread parallel_sum.o -L. -lsum_utils -o parallel_sum $(CFLAGS)

sum_utils.so: sum_utils.h
    $(CC) -fPIC -c sum_utils.c
    $(CC) -shared -o libsum_utils.so sum_utils.o



