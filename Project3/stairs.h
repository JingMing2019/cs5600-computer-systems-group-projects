#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>



typedef struct thread_arg {
    int index;
    int direction;  // 0 for up, 1 for down
} thread_arg_t;

// global constants
// define any global constants you want to use in your code

// #define the maximum number of customers/threads in the system to test
int MAX_CUSTOMERS = 30;
// #define how many customer can be on the stairs at the same time
int CUS_ON_STAIR = 1;
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm

// define timing variables



// thread
pthread_t *tid;



// Define global variables on the allowed direction, waiting threads, ...


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem;


// write any helper functions you need here
