#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <pthread.h>

//аргументы, передающиеся потоку
struct MulArgs {
	uint32_t first_number;
	uint32_t last_number;
	uint32_t mod;
	uint32_t *total_res;
	uint32_t th_num;
};

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

//это функция, которая передается в pthread_create
void *ThreadMul(void *args) {
	struct MulArgs *mul_args = (struct MulArgs *)args;
	uint32_t res = 1;
	uint32_t first_number = mul_args->first_number;
	uint32_t last_number = mul_args->last_number;
	for (uint32_t i = first_number; i <= last_number; i++)
	{
		res *= i;
		res %= mul_args->mod;
	}
	printf("thread%d res: %d", mul_args->th_num, res);
	pthread_mutex_lock(&mut);
	*(mul_args->total_res) *= res;
	*(mul_args->total_res) %= mul_args->mod;
	pthread_mutex_unlock(&mut);
	return (void *)(size_t)res;
}

int main(int argc, char **argv) {

	uint32_t threads_num = -1;
	printf("threads_num %d\n", threads_num);
	uint32_t k = -1;
	printf("k %d\n", k);
	uint32_t mod = -1;
	printf("mod %d\n", mod);
	int *total_res = malloc(sizeof(int) * 1);
	*total_res = 1;
    printf("total_res %d\n", *total_res);
    
    while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                      {"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
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
            k = atoi(optarg);
             if (k <= 0) {
                printf("k must be a positive number\n");
                return -1;
              }
            break;
          case 2:
            mod = atoi(optarg);
             if (mod <= 0) {
                printf("mod must be a positive number\n");
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
  
  
  if (k == -1 || mod == -1 || threads_num == -1) {
    printf("Usage: %s --threads_num \"num\" --k \"num\" --mod \"num\"\n",
           argv[0]);
    return 1;
}

	pthread_t threads[threads_num];

	uint32_t step = k / threads_num;
	printf("step %d\n", step);
	struct MulArgs args[threads_num];
	for (uint32_t i = 0; i < threads_num; i++) {
		args[i].first_number = i * step + 1;
		args[i].total_res = total_res;
		args[i].mod = mod;
		args[i].th_num = i;
		if (i == threads_num - 1)
		{
			args[i].last_number = k;
		}
		else
		{
			args[i].last_number = i * step + step;
		}
		if (pthread_create(&threads[i], NULL, ThreadMul, (void *)&args[i])) {
			printf("Error: pthread_create failed!\n");
			return 1;
		}
		printf("i:%d\n", i);
	}

	uint32_t threads_n = threads_num;
	uint32_t res = 1;
	for (uint32_t i = 0; i < threads_n; i++) {
		int status = pthread_join(threads[i], (void **)&res);
		printf("status:%d\n", status);
	}

	uint32_t check_fact = 1;
	for (uint32_t i = 1; i <= k; i++)
	{
		check_fact *=i;
		check_fact %= mod;
	}
	printf("Check2: %d\n", check_fact);
	printf("Total: %d\n", *total_res);
	return 0;
}
