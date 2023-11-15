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
// Changing them should be guarded by a mutex.
int num_stairs; // user input number of stairs
// Define direction of set of customers currently crossing the stairs. Initalize
// it as -1 means currently no customers on the stairs.
int current_direction = -1;

// initialize the mutex with the constant which is provided by pthread.h library
// to set all the fields of the mutex attributes structure to default values
// `release_semaphore_mutex` makes sure only one thread can increase semaphore
// value at the same time.
pthread_mutex_t release_semaphore_mutex = PTHREAD_MUTEX_INITIALIZER;
// `direction_change_mutex` guards the global variable current_direciotn.
pthread_mutex_t change_current_direction_mutex = PTHREAD_MUTEX_INITIALIZER;

// Define semaphore
sem_t available_stairs_sem;
sem_t is_stairs_empty_sem;


// write any helper functions you need here
int semwait(sem_t *sem);
int sempost(sem_t *sem);
void *threadfunction(void *vargp);
void cleanup();