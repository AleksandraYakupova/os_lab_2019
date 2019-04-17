#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include <pthread.h>
#include "utils.h"
#include "sum_utils.h"

//это функция, которая передается в pthread_create
void *ThreadSum(void *args) {
	struct SumArgs *sum_args = (struct SumArgs *)args;
	//printf("th_num %d\n",sum_args->th_num);
	return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
	/*
	*  TODO:
	*  threads_num by command line arguments
	*  array_size by command line arguments
	*	seed by command line arguments
	*/

	uint32_t threads_num = -1;
	uint32_t array_size = -1;
	uint32_t seed = -1;
  
 while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"seed", required_argument, 0, 0},
                                      {0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "",options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            threads_num = atoi(optarg);
            if (threads_num <= 0) {
                printf("threads_num must be a positive number\n");
                return -1;
              }
            break;
          case 1:
            array_size = atoi(optarg);
             if (array_size <= 0) {
                printf("array_size must be a positive number\n");
                return -1;
              }
            break;
          case 2:
            seed = atoi(optarg);
             if (seed <= 0) {
                printf("seed must be a positive number\n");
                return -1;
              }
            break;
            defalut:
            printf("Index %d is out of options\n", option_index);
        }

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
    
  }
  
  
  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --threads_num \"num\" --seed \"num\" --array_size \"num\"\n",
           argv[0]);
    return 1;
}
  	printf("threads_num %d\n",threads_num);
  	printf("seed %d\n",seed);
  	printf("array_size %d\n",array_size);
  	pthread_t threads[threads_num];

	int *array = malloc(sizeof(int) * array_size);
	GenerateArray(array, array_size, seed);
	//for (int i = 0; i < array_size; i++)
	//{
		//array[i] = i;
	//}
	
	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	int step = array_size / threads_num;
	printf("step %d\n",step);
	//каждому потоку потоку передается один и тот же массив, на разные begin,end
	//здесь происходит деление на части?
	struct SumArgs args[threads_num];
	for (uint32_t i = 0; i < threads_num; i++) {
	    args[i].th_num = (int)i;
		args[i].begin = i * step;
		if (i == threads_num - 1)
		{
		    args[i].end = array_size; 
		}
		else
		{
		    args[i].end = i * step + step; 
		}
		args[i].array = array;
		if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
			printf("Error: pthread_create failed!\n");
			return 1;
		}
		printf("i:%d\n",i);
	}

	int total_sum = 0;
	printf("threads_num %d\n", threads_num);
	for (uint32_t i = 0; i < threads_num; i++) {
		printf("Sum%d \n", i);
	}
	uint32_t threads_n = threads_num;
	int sum = 0;
	for (uint32_t i = 0; i < threads_n; i++) {
		//printf("cycle threads_num:%d\n",threads_num);
		
		//ожидаем от каждого потока сумму
		int status = pthread_join(threads[i], (void **)&sum);
		printf("status:%d\n",status);
		//if (status != 0) {
        //printf("main error: can't join thread, status = %d\n", status);
        //exit(ERROR_JOIN_THREAD);
    //}
		//printf("Sum%d: %d\n", i, sum);
		total_sum += sum;
	}
	
	struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
	
	int check_sum = 0;
	for (int i = 0; i < array_size; i++)
	{
		check_sum += array[i];
	}
	printf("Check: %d\n", check_sum);
	free(array);
	printf("Total: %d\n", total_sum);
	printf("Elapsed time: %fms\n", elapsed_time);
	return 0;
}
