#include "stairs.h"
#include <pthread.h>
#include <semaphore.h>

int upCount = 0;
int downCount = 0;


// You can write your own semwait function that can call sem_wait(sem) or sem_trywait(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
void semwait(sem_t *sem) {
    pthread_mutex_lock(&mutex);
    if (on_stairs >= MAX_CUS_ON_STAIRS) {
        pthread_mutex_unlock(&mutex);
        sem_wait(sem);
    } else {
        on_stairs++;
        pthread_mutex_unlock(&mutex);
    }
}

// You can write your own sempost function that call sem_post(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
void sempost(sem_t *sem) {
    pthread_mutex_lock(&mutex);
    on_stairs--;
    if (on_stairs == 0) {
        sem_post(sem) // Signal that the stairs are free
    }
    pthread_mutex_unlock(&mutex);
}

void *threadfunction (void *vargp) {
    // write the threadfunction whose name should be part of pthread_create(..)
	// Don't forget your timing computations
    thread_arg_t *t_arg = (thread_arg_t*)vargp;
    int id = t_arg->index;
    int direction = t_arg->direction;

    sem_wait(&sem);

    if (direction == 0) {
      if (downCount > 0) {
        printf("Customer %d going up should wait\n", id);
        // release and then re-acquire?
      }
      upCount++;
    } else {
      if (upCount > 0) {
        printf("Customer %d going down should wait\n", id);
        // release and then re-acquire
      }
      downCount++;
    }

    sem_post(&sem);

    // start_time
    printf("Customer %d crossing the stairs now \n", id);
    sleep(rand() % 10);
    // end_time

    sem_wait(&sem);

    if (direction == 0) {
      upCount--;
    } else {
      downCount--;
    }

    sem_post(&sem);
    printf("Customer %d finished stairs \n", id);

    // calculate turnaround time

    pthread_exit(NULL);
}

// function to clean up thread, mutex and semaphore
void cleanup() {
    free(tid);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
}

int main(int argc, char *argv[]) {


    //printf("Number of Customers: %d\nNumber of stairs: %d\n", ...., .....);



    // sem_init(.....);
	// generate an array of threads, set their direction randomly, call pthread_create,
	// then sleep for some random nonzero time

  // your code here

	// for each thread created, call pthread_join(..)




   // printf turnaround time for each thread and average turnaround time

  // free every pointer you used malloc for


    return 0;
}
