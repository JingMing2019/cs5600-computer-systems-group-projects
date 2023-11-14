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
    struct timeval start_time; // to store the thread start time
    struct timeval end_time; // to store the thread end time
} thread_arg_t;

// global constants
// define any global constants you want to use in your code

// #define the maximum number of customers/threads in the system to test
int MAX_CUSTOMERS = 30;
// define the maximum number of steps(or stairs)
int MAX_STAIRS = 13;
// #define how many customer can be on the stairs at the same time
int MAX_CUS_ON_STAIRS = 1;
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm


// define timing variables


// thread
pthread_t *tid;

// Define global variables on the allowed direction, waiting threads, ...

// initialize the mutex with the constant which is provided by pthread.h library
// to set all the fields of the mutex attributes structure to default values
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// initialize a semaphore
sem_t sem;
// define the number of customers currently on the stairs
int on_stairs = 0;


// write any helper functions you need here
void semwait(sem_t *sem);
void sempost(sem_t *sem);
void *threadfunction(void *vargp);
void cleanup();