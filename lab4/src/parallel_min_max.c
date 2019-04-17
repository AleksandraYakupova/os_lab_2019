
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t first_pid;
void send_sigkill(int var)
{
    printf("kill");
    killpg(first_pid, SIGKILL);//посылаем сигнал всем процессам в группе
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  int timeout = -1;
  int waiting = -1;
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 't'},
                                      {"waiting", required_argument, 0, 'w'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "ftw",options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed must be a positive number\n");
                return 1;
              }
            break;
          case 1:
            array_size = atoi(optarg);
             if (array_size <= 0) {
                printf("array_size must be a positive number\n");
                return 1;
              }
            break;
          case 2:
            pnum = atoi(optarg);
             if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
              }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
    
      case 't':
        timeout = atoi(optarg);
        if (timeout <= 0) {
                printf("timeout must be a positive number\n");
                return 1;
          }
        break;
        
      case 'w':
        waiting = atoi(optarg);
        if (timeout <= 0) {
                printf("waiting must be a positive number\n");
                return 1;
          }
        break;
        
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
    
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

    int part = array_size/pnum;
    int file_pipes[2];
    
    pipe(file_pipes);
      
  //setpgid(0, 0);
  signal(SIGALRM, send_sigkill);
  alarm(0.00002);
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes++;
      if (child_pid == 0) {
        if (i == 0) 
        {
            setpgid(0, 0);
            first_pid = getpid();
        }
        else
        {
            setpgid(0, first_pid);
        }
         struct MinMax mm;
         if(i*part < array_size)
             mm = GetMinMax(array + i*part, 
                            0,
                            part);
         else
            mm = GetMinMax(array + array_size - part, 0, part);

        if (with_files) {
          FILE* fp = fopen("numbers.txt", "a");
          fwrite(&mm, sizeof(struct MinMax), 1, fp);
          fclose(fp);
        } else {
          // use pipe here
          write(file_pipes[1],&mm,sizeof(struct MinMax));
          
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

    
  while (active_child_processes > 0) {
        // your code here
    close(file_pipes[1]);
    wait(NULL);//ждем, пока дочерний процесс завершится, не записываем инфу о статусе завершения
    active_child_processes -= 1;
    }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;


  for (int i = 0; i < pnum; i++) {
    struct MinMax mm;
    
    if (with_files) {
      // read from files
        FILE* fp = fopen("numbers.txt", "rb");
        fseek(fp, i*sizeof(struct MinMax), SEEK_SET);
        fread(&mm, sizeof(struct MinMax), 1, fp);
        fclose(fp);
    } else {
      // read from pipes
      read(file_pipes[0], &mm, sizeof(struct MinMax));
    }

    if (mm.min < min_max.min) min_max.min = mm.min;
    if (mm.max > min_max.max) min_max.max = mm.max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  
  return 0;
}

