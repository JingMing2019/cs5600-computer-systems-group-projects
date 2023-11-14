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

    // time_t start_time = time(NULL);
    printf("Customer %d crossing the stairs now \n", id);
    sleep(rand() % 10);
    // time_t end_time = time(NULL);

    semwait(&sem);

    if (direction == 0) {
      upCount--;
    } else {
      downCount--;
    }

    sempost(&sem);
    printf("Customer %d finished stairs \n", id);

    // time_t turnaround = end_time - start_time;
    // printf("turnaround time is %ld seconds\n", turnaround);

    pthread_exit(NULL);
}

// function to clean up thread, mutex and semaphore
void cleanup() {
    free(tid);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
}

int main(int argc, char *argv[]) {

    int num_customers = atoi(argv[1]);
    int num_stairs = atoi(argv[2]);

    // // Prompt the user to input number of customers and stairs they would like
    // // to test.
    // printf("Enter number of customers (smaller or equal to %d):", MAX_CUSTOMERS);
    // scanf("%d\n", &num_customers);

    // printf("Enter number of stairs (smaller or euqal to %d):", MAX_STAIRS);
    // scanf("%d\n", &num_stairs);

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

    tid = (pthread_t *)malloc(sizeof(pthread_t) * num_customers);
    // Seed the random number generator as current time.
    srand(time(NULL));
    time_t start_time[num_customers];

    for (int i = 0; i < num_customers; i++) {
        // Set direction as random 0 or 1.
        int rand_direction = rand() % 2;
        thread_arg_t args = {i, rand_direction};
        printf("Customer %d goes up or down (0 for up, 1 for down): %d\n", 
            i, rand_direction);
        if (pthread_create(&tid[i], NULL, threadfunction, &args)) {
            printf("Error occurs in thread creation.");
            exit(1);
        }
        start_time[i] = time(NULL);
        sleep(rand() % 5 + 1);
    }


    // for each thread created, call pthread_join(..)
    time_t end_time[num_customers];
    for (int i = 0; i < num_customers; i++) {
        pthread_join(tid[i], NULL);
        end_time[i] = time(NULL);
    }

   // printf turnaround time for each thread and average turnaround time
    time_t turnaround[num_customers];
    double sum_turnaround;

    for (int i = 0; i < num_customers; i++) {
        turnaround[i] = end_time[i] - start_time[i];
        printf("Customer %d turnaround time is %.4f seconds.\n", i, 
            (double) turnaround[i]);
        sum_turnaround += (double) turnaround[i];
    }

    double avg_turnaround = sum_turnaround / num_customers;
    printf("Average turnaround time is %.4f seconds.\n", avg_turnaround);


  // free every pointer you used malloc for
    cleanup();

    return 0;
}
