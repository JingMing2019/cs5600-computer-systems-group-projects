#include "stairs.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

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
        sem_post(sem); // Signal that the stairs are free
    }
    pthread_mutex_unlock(&mutex);
}

void *threadfunction (void *vargp) {
    // write the threadfunction whose name should be part of pthread_create(..)
	// Don't forget your timing computations
    thread_arg_t *t_arg = (thread_arg_t*)vargp;

    // Record the start time.
    gettimeofday(&t_arg->start_time, NULL);

    int id = t_arg->index;
    int direction = t_arg->direction;

    semwait(&sem);

    if (direction == 0) {
      if (downCount > 0) {
        printf("Customer %d going up should wait\n", id);
        sempost(&sem);
        semwait(&sem);
      }
      upCount++;
    } else {
      if (upCount > 0) {
        printf("Customer %d going down should wait\n", id);
        sempost(&sem);
        semwait(&sem);
      }
      downCount++;
    }

    sempost(&sem);

    printf("Customer %d crossing the stairs now \n", id);
    sleep(rand() % 10);

    semwait(&sem);

    if (direction == 0) {
      upCount--;
    } else {
      downCount--;
    }

    sempost(&sem);
    printf("Customer %d finished stairs \n", id);

    // Record the end time.
    gettimeofday(&t_arg->end_time, NULL);

    pthread_exit(NULL);
}

// function to clean up thread, mutex and semaphore
void cleanup() {
    free(tid);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
}

int main(int argc, char *argv[]) {
    // Get number of customers and number of stairs from command-line arguments.
    int num_customers = atoi(argv[1]);
    int num_stairs = atoi(argv[2]);

    if (num_customers > MAX_CUSTOMERS) {
        printf("Error Input %d. Number of customers should not exceed %d.\n", 
            num_customers, MAX_CUSTOMERS);
        exit(1);
    }

    if (num_stairs > MAX_STAIRS) {
        printf("Error Input %d. Number of stairs should not exceed %d.\n", 
            num_stairs, MAX_STAIRS);
        exit(1);
    }
   
    printf("Number of Customers: %d\nNumber of stairs: %d\n", num_customers, 
        num_stairs);

    // Initializes the semaphore. Set the pshared as 0 so that this semaphore is 
    // shared between the threads of a process. Set value as `num_stairs` means
    // initially there are `num_stairs` empty spaces for customers to use.
    sem_init(&sem, 0, num_stairs);

	// generate an array of threads, set their direction randomly, call pthread_create,
	// then sleep for some random nonzero time
    // Allocate memory for each thread.
    tid = (pthread_t *)malloc(sizeof(pthread_t) * num_customers);
    // Define an array of thread arguments.
    thread_arg_t args[num_customers];
    // Seed the random number generator as current time.
    srand(time(NULL));

    for (int i = 0; i < num_customers; i++) {
        // Set thread index as i.
        args[i].index = i;
        // Set direction as random 0 or 1.
        args[i].direction = rand() % 2;
        printf("Customer %d goes up or down (0 for up, 1 for down): %d\n", 
            args[i].index, args[i].direction);
        if (pthread_create(&tid[i], NULL, threadfunction, (void *)&args[i])) {
            printf("Error occurs in thread creation.");
            exit(1);
        }
        // Sleep for random 1 to 10 time.
        sleep(rand() % 10 + 1);
    }

    // for each thread created, call pthread_join(..)
    for (int i = 0; i < num_customers; i++) {
        pthread_join(tid[i], NULL);
    }

    // printf turnaround time for each thread and average turnaround time
    double turnaround[num_customers];
    // Initialize the sum of turnaround as 0.
    double sum_turnaround = 0;

    for (int i = 0; i < num_customers; i++) {
        turnaround[i] = (args[i].end_time.tv_sec - args[i].start_time.tv_sec) +
         (args[i].end_time.tv_usec - args[i].start_time.tv_usec) * 1e-6;
        printf("Customer %d turnaround time is %.4f seconds.\n", i, 
            turnaround[i]);
        sum_turnaround += turnaround[i];
    }

    double avg_turnaround = sum_turnaround / num_customers;
    printf("Average turnaround time is %.4f seconds.\n", avg_turnaround);

  // free every pointer you used malloc for
    cleanup();

    return 0;
}
