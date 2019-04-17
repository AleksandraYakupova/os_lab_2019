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

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

//struct ThreadArgs {
    //int a;
    //int b;
//}

bool inside_loop = false;
int b = 0;
void *ThreadFunc(void *args) {
    printf("a\n");
        int *a = (int *)args;
        pthread_mutex_lock(&mut);
        printf("a is now locked\n");
        while(*a == 0) {
            inside_loop = true;
            printf("check a again\n");
            sleep(2);
        }
        pthread_mutex_unlock(&mut);
        printf("a is now unlocked\n");
        b = 1;
        printf("b is changed\n");
}

int main() {
    //ThreadArgs args;
    //args.a = 0;
    //args.b = 1;
    int a = 0;
    pthread_t thread;
    printf("creating new thread\n");
    pthread_create(&thread, NULL, ThreadFunc, &a);
    while (!inside_loop)
    {
        sleep(5);
    }
    printf("inside_loop is true now\n");
    while (b == 0)
    {
        printf("check b again\n");
        sleep(2);
    }
    a = 1;
    printf("a is changed\n");
    return 0;

}
